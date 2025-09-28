document.addEventListener("DOMContentLoaded", () => {
    let backend = null;
    let radarNodes = [];

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
        radarNodes = networks.map(n => ({ ...n, x: 0, y: 0 }));

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
                    <div style="width:78px;text-align:right;color:var(--muted);font-size:12px">${-n.signal} dBm</div>
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
        infoDist.textContent = 'Unknown'; // Update later dynamically
        strengthMeter.style.width = '60%'; // Placeholder or from network info
        integrityMeter.style.width = '70%'; // Placeholder or from network info
        status.textContent = 'selected';
        logLine(`Selected network ${n.ssid} (${n.bssid})`);

        // Store selected network globally for button action context
        window.selectedNetwork = n;
    }

    function renderRadar() {
        const radarEl = document.getElementById('radar');

        radarEl.querySelectorAll('.node').forEach(n => n.remove());

        const rMax = 150;
        const centerX = 180;
        const centerY = 120;

        radarNodes.forEach(n => {
            const node = document.createElement('div');
            node.className = 'node';

            const normalized = Math.max(-90, Math.min(-30, n.signal));
            const distance = rMax * (1 - (normalized + 90) / 60);

            if (n.angle === undefined) n.angle = Math.random() * 2 * Math.PI;

            n.x = centerX + distance * Math.cos(n.angle);
            n.y = centerY + distance * Math.sin(n.angle);

            node.style.left = `${n.x}px`;
            node.style.top = `${n.y}px`;

            if (n.signal >= -50)
                node.style.background = 'linear-gradient(180deg,#00ffd5,#00a8ff)';
            else if (n.signal >= -70)
                node.style.background = 'linear-gradient(180deg,#00ff85,#00ffd5)';
            else
                node.style.background = 'linear-gradient(180deg,#ff4b6e,#ff85aa)';

            node.title = `${n.ssid} (${n.signal} dBm)`;

            radarEl.appendChild(node);
        });
    }

    function startRadarAnimation() {
        function loop() {
            renderRadar();
            requestAnimationFrame(loop);
        }
        requestAnimationFrame(loop);
    }

    function enableFlux() {
      if (!window.selectedNetwork) {
        logLine("No network selected for flux monitoring", "warn");
        return;
      }
      logLine(`Starting monitor on ${window.selectedNetwork.ssid} (Flux)`);
      loadModeContent(); // Switch UI from radar to attack screen
      backend.startMonitor(window.selectedNetwork.bssid);
    }

    function enableNoise() {
        if (!window.selectedNetwork) {
            logLine("No network selected for noise capture", "warn");
            return;
        }
        logLine(`Starting handshake capture on ${window.selectedNetwork.ssid} (Noise)`);
        backend.startCapture(window.selectedNetwork.bssid);
        // UI and state update logic
    }

    function probeCrack() {
        if (!window.selectedNetwork) {
            logLine("No network selected for probe/crack", "warn");
            return;
        }
        // Show modal or prompt for cracking options
        logLine(`Probe cracking options for ${window.selectedNetwork.ssid}`);
        // Example: simple prompt - expand later to modal UI
        const crackingType = prompt("Enter cracking method: wordlist or brute", "wordlist");
        if(crackingType) {
            backend.startCracking(crackingType);
        }
    }
    function loadModeContent() {
      fetch('mode.html')
        .then(response => response.text())
        .then(html => {
          const container = document.getElementById('radarContainer');
          if (container) {
            container.innerHTML = html;
          }
        })
        .catch(err => logLine(`Failed to load mode.html: ${err}`, 'error'));
    }
    function loadRadarContent() {
      const radarHtml = `
        <div class="radar-canvas" id="radar">
          <div class="radar-grid" aria-hidden="true">
            <div class="circle c2"></div>
            <div class="circle c1"></div>
          </div>
          <div class="sweep" aria-hidden="true"></div>
          <div class="radar-center" title="You"></div>
        </div>
      `;
      const container = document.getElementById('radarContainer');
      if (container) {
        container.innerHTML = radarHtml;
      }
    }

    if (typeof qt !== "undefined") {
        new QWebChannel(qt.webChannelTransport, function(channel) {
            backend = channel.objects.backend;

            backend.scanResults.connect(networks => {
                renderNetworks(networks);
            });
            backend.logMessage.connect(text => logLine(text));

            document.getElementById('scanBtn').onclick = () => {
                backend.startScan();
            };

            document.getElementById('clearBtn').onclick = () => {
              backend.clear();
              radarNodes = [];
              loadRadarContent(); // Restore radar UI
            };

            document.getElementById('floodBtn').onclick = enableFlux;
            document.getElementById('noiseBtn').onclick = enableNoise;
            document.getElementById('probeBtn').onclick = probeCrack;
        });
    } else {
        logLine("Qt WebChannel not detected", "error");
    }

    startRadarAnimation();
});
