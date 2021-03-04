/*
 * Copyright 2008 Search Solution Corporation
 * Copyright 2016 CUBRID Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#include "active_tran_server.hpp"

#include "communication_server_channel.hpp"
#include "error_manager.h"
#include "log_impl.h"
#include "log_prior_send.hpp"
#include "server_type.hpp"

#include <cassert>
#include <functional>

active_tran_server ats_Gl;

static void assert_is_active_tran_server ();

active_tran_server::~active_tran_server ()
{
  disconnect_page_server ();
}

void
active_tran_server::init_page_server_hosts (const char *db_name)
{
  assert_is_active_tran_server ();

  std::string hosts = prm_get_string_value (PRM_ID_PAGE_SERVER_HOSTS);

  if (!hosts.length ())
    {
      return;
    }

  auto col_pos = hosts.find (":");

  if (col_pos < 1 || col_pos >= hosts.length () - 1)
    {
      er_set (ER_WARNING_SEVERITY, ARG_FILE_LINE, ER_HOST_PORT_PARAMETER, 2, prm_get_name (PRM_ID_PAGE_SERVER_HOSTS),
	      hosts.c_str ());
      return;
    }

  long port = -1;
  try
    {
      port = std::stol (hosts.substr (col_pos+1));
    }
  catch (...)
    {
    }

  if (port < 1 || port > USHRT_MAX)
    {
      er_set (ER_WARNING_SEVERITY, ARG_FILE_LINE, ER_HOST_PORT_PARAMETER, 2, prm_get_name (PRM_ID_PAGE_SERVER_HOSTS),
	      hosts.c_str ());
      return;
    }
  m_ps_port = port;

  // host and port seem to be OK
  m_ps_hostname = hosts.substr (0, col_pos);
  er_log_debug (ARG_FILE_LINE, "Page server hosts: %s port: %d\n", m_ps_hostname.c_str (), m_ps_port);

  connect_to_page_server (m_ps_hostname, m_ps_port, db_name);
}

int
active_tran_server::connect_to_page_server (const std::string &host, int port, const char *db_name)
{
  assert_is_active_tran_server ();
  assert (!is_page_server_connected ());

  // connect to page server
  cubcomm::server_channel srv_chn (db_name);

  srv_chn.set_channel_name ("ATS_PS_comm");

  css_error_code comm_error_code = srv_chn.connect (host.c_str (), port, CMD_SERVER_SERVER_CONNECT);
  if (comm_error_code != css_error_code::NO_ERRORS)
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_NET_PAGESERVER_CONNECTION, 1, host.c_str());
      return ER_NET_PAGESERVER_CONNECTION;
    }

  if (!srv_chn.send_int (static_cast <int> (cubcomm::server_server::CONNECT_ACTIVE_TRAN_TO_PAGE_SERVER)))
    {
      er_set (ER_ERROR_SEVERITY, ARG_FILE_LINE, ER_NET_PAGESERVER_CONNECTION, 1, host.c_str());
      return ER_NET_PAGESERVER_CONNECTION;
    }

  page_server_conn ps_conn (std::move (srv_chn));
  m_ps_request_queue = new page_server_request_queue (std::move (ps_conn));
  m_ps_request_autosend = new page_server_request_autosend (*m_ps_request_queue);

  log_Gl.m_prior_sender.add_sink (std::bind (&active_tran_server::push_request, std::ref (*this),
				  ats_to_ps_request::SEND_LOG_PRIOR_LIST, std::placeholders::_1));
}

void
active_tran_server::disconnect_page_server ()
{
  assert_is_active_tran_server ();

  delete m_ps_request_autosend;
  m_ps_request_autosend = nullptr;

  delete m_ps_request_queue;
  m_ps_request_queue = nullptr;
}

bool
active_tran_server::is_page_server_connected () const
{
  assert_is_active_tran_server ();
  return m_ps_request_queue != nullptr;
}

void
active_tran_server::push_request (ats_to_ps_request reqid, std::string &&payload)
{
  assert (is_page_server_connected ());

  m_ps_request_queue->push (reqid, std::move (payload));
}

void
assert_is_active_tran_server ()
{
  assert (get_server_type () == SERVER_TYPE::SERVER_TYPE_TRANSACTION);
}