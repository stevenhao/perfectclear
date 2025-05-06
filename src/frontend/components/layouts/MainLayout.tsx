import React, { useEffect, useState } from 'react';
import { Outlet } from '@tanstack/react-router';
import { fetchServerStatus } from '../../services/api';

interface ServerStatus {
  connected: boolean;
  uptime: number;
}

export function MainLayout() {
  const [serverStatus, setServerStatus] = useState<ServerStatus>({
    connected: false,
    uptime: 0
  });

  useEffect(() => {
    const checkServerStatus = async () => {
      try {
        const status = await fetchServerStatus();
        setServerStatus(status);
      } catch (error) {
        console.error('Error fetching server status:', error);
      }
    };

    checkServerStatus();
    const interval = setInterval(checkServerStatus, 5000);
    return () => clearInterval(interval);
  }, []);

  return (
    <>
      <nav className="navbar navbar-default">
        <div className="container-fluid">
          <div className="navbar-header">
            <button
              type="button"
              className="navbar-toggle collapsed"
              data-toggle="collapse"
              data-target="#bs-example-navbar-collapse-1"
              aria-expanded="false"
            >
              <span className="sr-only">Toggle navigation</span>
              <span className="icon-bar"></span>
              <span className="icon-bar"></span>
              <span className="icon-bar"></span>
            </button>
            <a className="navbar-brand" href="#">Bbbb</a>
          </div>
          <div className="collapse navbar-collapse" id="bs-example-navbar-collapse-1">
            <ul className="nav navbar-nav navbar-right">
              <li>
                <a href="#" id="server-status-indicator">
                  Server: <span id="server-status-light" className={`status-light ${serverStatus.connected ? 'connected' : 'disconnected'}`}>●</span>
                  <span id="server-uptime">{serverStatus.connected ? ` ${Math.floor(serverStatus.uptime / 1000)}s` : ''}</span>
                </a>
              </li>
            </ul>
          </div>
        </div>
      </nav>
      <div className="container">
        <Outlet />
      </div>
    </>
  );
}
