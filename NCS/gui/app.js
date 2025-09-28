document.addEventListener("DOMContentLoaded", () => {
  let backend = null;

  function logLine(text, tone='info') {
    const term = document.getElementById('term');
    const div = document.createElement('div');
    div.textContent = `[${new Date().toLocaleTimeString()}] ${text}`;
    if(tone === 'warn') div.style.color = '#ffb3bd';
    if(tone === 'error') div.style.color = '#ff8fa0';
    term.appendChild(div);
    term.scrollTop = term.scrollHeight;

    while(term.children.length > 220) term.removeChild(term.firstChild);
  }

  function renderNetworks(networks) {
    const netListEl = document.getElementById('netList');
    netListEl.innerHTML = '';
    networks.sort((a,b) => b.signal - a.signal);
    networks.forEach(n => {
      const netEl = document.createElement('div');
      netEl.className = 'net';
      netEl.innerHTML = `
        <div class="left">
          <div style="width:44px;height:44px;border-radius:8px;background:linear-gradient(135deg,rgba(0,168,255,0.07),rgba(0,255,213,0.03));display:flex;align-items:center;justify-content:center;font-weight:700">${n.ssid[0] || 'W'}</div>
          <div>
            <div class="ssid">${n.ssid}</div>
            <div class="meta">${n.security} • ch ${n.channel} • ${n.bssid}</div>
          </div>
        </div>
        <div style="display:flex;align-items:center;gap:12px">
          <div class="bars" title="Signal">
            <div class="bar h1 ${n.signal > -80 ? 'on' : ''}"></div>
            <div class="bar h2 ${n.signal > -70 ? 'on' : ''}"></div>
            <div class="bar h3 ${n.signal > -60 ? 'on' : ''}"></div>
            <div class="bar h4 ${n.signal > -50 ? 'on' : ''}"></div>
          </div>
          <div style="width:78px;text-align:right;color:var(--muted);font-size:12px">${ -n.signal } dBm</div>
        </div>`;
      netEl.addEventListener('click', () => selectNetwork(n));
      netListEl.appendChild(netEl);
    });
  }

  function selectNetwork(n) {
    const infoTitle = document.getElementById('infoTitle');
    const infoSSID = document.getElementById('infoSSID');
    const infoCh = document.getElementById('infoCh');
    const infoBssid = document.getElementById('infoBssid');
    const infoAuth = document.getElementById('infoAuth');
    const infoSignal = document.getElementById('infoSignal');
    const infoDist = document.getElementById('infoDist');
    const strengthMeter = document.getElementById('strengthMeter');
    const integrityMeter = document.getElementById('integrityMeter');
    const status = document.getElementById('status');

    infoTitle.textContent = `Selected: ${n.ssid}`;
    infoSSID.textContent = n.ssid;
    infoCh.textContent = n.channel;
    infoBssid.textContent = n.bssid;
    infoAuth.textContent = n.security;
    infoSignal.textContent = `${-n.signal} dBm`;
    infoDist.textContent = 'Unknown';
    strengthMeter.style.width = '60%';
    integrityMeter.style.width = '70%';
    status.textContent = 'selected';
    logLine(`Selected network ${n.ssid} (${n.bssid})`);
  }

  if (typeof qt !== "undefined") {
    new QWebChannel(qt.webChannelTransport, function(channel) {
      backend = channel.objects.backend;

      backend.scanResults.connect(renderNetworks);
      backend.logMessage.connect(text => logLine(text));

      document.getElementById('scanBtn').onclick = () => {
        backend.startScan();
      };

      document.getElementById('clearBtn').onclick = () => {
        backend.clear();
      };
    });
  } else {
    logLine("Qt WebChannel not detected", "error");
  }
});
