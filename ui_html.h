#pragma once

static const char* kCompiledUiHtml = R"CEL_UI_EOF(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Celestial Skin Changer</title>
<style>
@import url('https://fonts.googleapis.com/css2?family=Chakra+Petch:wght@500;600;700&family=JetBrains+Mono:wght@500;700&family=Space+Grotesk:wght@500;600;700&display=swap');

:root {
  --bg-void: #090710;
  --bg-panel: #110d1b;
  --bg-card: #171226;
  --bg-elev: #211a36;
  --bg-hover: #2c2347;
  --border: #2b2244;
  --border-hi: #4a3a75;
  --cel-purple: #9333ea;
  --cel-purple-hi: #a855f7;
  --cel-lavender: #d8b4fe;
  --text-main: #f5f3ff;
  --text-sub: #a19bb5;
  --text-muted: #6e6785;
  --danger: #e11d48;
  --success: #10b981;
  --cloud-blue: #0ea5e9;
  --cloud-blue-hi: #38bdf8;
  --font-display: 'Chakra Petch', 'Space Grotesk', 'Bahnschrift', 'Segoe UI', sans-serif;
  --font-sans: 'Space Grotesk', 'Bahnschrift', 'Segoe UI', system-ui, sans-serif;
  --font-mono: 'JetBrains Mono', 'Consolas', monospace;
  --cut-sm: polygon(0 0, calc(100% - 7px) 0, 100% 7px, 100% 100%, 7px 100%, 0 calc(100% - 7px));
  --cut-md: polygon(0 0, calc(100% - 12px) 0, 100% 12px, 100% 100%, 12px 100%, 0 calc(100% - 12px));
  --cut-lg: polygon(0 0, calc(100% - 16px) 0, 100% 16px, 100% 100%, 16px 100%, 0 calc(100% - 16px));
  color-scheme: dark;
  box-sizing: border-box;
}
*, *::before, *::after { box-sizing: inherit; }
[hidden] { display: none !important; }

html, body {
  position: fixed;
  inset: 0;
  width: 100%;
  height: 100%;
  margin: 0 !important;
  padding: 0 !important;
  background: var(--bg-void) !important;
  color: var(--text-main);
  font: 500 13px/1.4 var(--font-sans);
  overflow: hidden !important;
  user-select: none;
  -webkit-font-smoothing: antialiased;
}

button, input { font: inherit; color: inherit; }

::-webkit-scrollbar { width: 10px; height: 10px; }
::-webkit-scrollbar-track { background: transparent; }
::-webkit-scrollbar-thumb {
  background-color: var(--bg-elev);
  border: 2px solid var(--bg-panel);
  border-radius: 4px;
}
::-webkit-scrollbar-thumb:hover { background-color: var(--cel-purple); }

.app {
  position: fixed;
  inset: 0;
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  background: var(--bg-void);
  border: 1px solid var(--border);
  overflow: hidden;
  z-index: 2147483647;
}

/* Custom Titlebar */
.titlebar {
  height: 42px;
  min-height: 42px;
  background: var(--bg-panel);
  border-bottom: 1px solid var(--border);
  display: flex;
  align-items: center;
  padding: 0 12px 0 16px;
  gap: 12px;
  flex-shrink: 0;
  -webkit-app-region: drag;
}
.no-drag, .titlebar button, .titlebar input {
  -webkit-app-region: no-drag !important;
}
.brand { display: flex; align-items: center; gap: 10px; }
.brand-avatar {
  width: 24px;
  height: 24px;
  object-fit: cover;
  object-position: right top;
  border: 1px solid var(--cel-purple-hi);
  clip-path: var(--cut-sm);
}
.brand-name {
  font-family: var(--font-display);
  font-size: 15px;
  font-weight: 700;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: var(--text-main);
}
.brand-tag {
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  padding: 2px 8px;
  background: var(--bg-elev);
  border: 1px solid var(--border-hi);
  color: var(--cel-lavender);
  clip-path: var(--cut-sm);
}

.nav-tabs { display: flex; gap: 6px; margin-left: 12px; }
.nav-btn {
  height: 28px;
  padding: 0 14px;
  background: var(--bg-card);
  border: 1px solid var(--border);
  color: var(--text-sub);
  font-family: var(--font-display);
  font-size: 12px;
  font-weight: 700;
  letter-spacing: 0.06em;
  text-transform: uppercase;
  cursor: pointer;
  clip-path: var(--cut-sm);
}
.nav-btn:hover { background: var(--bg-hover); color: var(--text-main); }
.nav-btn.is-active { background: var(--cel-purple); border-color: var(--cel-purple-hi); color: #fff; }

.tb-actions { margin-left: auto; display: flex; align-items: center; gap: 8px; }
.btn-cyber {
  height: 28px;
  padding: 0 13px;
  background: var(--bg-elev);
  border: 1px solid var(--border-hi);
  color: var(--text-main);
  font-family: var(--font-display);
  font-size: 11px;
  font-weight: 700;
  letter-spacing: 0.06em;
  text-transform: uppercase;
  cursor: pointer;
  clip-path: var(--cut-sm);
}
.btn-cyber:hover { background: var(--bg-hover); border-color: var(--cel-lavender); }
.btn-sync {
  background: #0c2d48;
  border-color: var(--cloud-blue);
  color: var(--cloud-blue-hi);
}
.btn-sync:hover { background: var(--cloud-blue); color: #050508; }
.btn-primary { background: var(--cel-purple); border-color: var(--cel-purple-hi); color: #fff; }
.btn-primary:hover { background: var(--cel-purple-hi); }
.btn-primary.is-running { background: var(--danger); border-color: var(--danger); }

.win-ctrls {
  display: flex;
  align-items: center;
  gap: 4px;
  margin-left: 6px;
  padding-left: 8px;
  border-left: 1px solid var(--border);
}
.win-btn {
  width: 28px;
  height: 26px;
  background: transparent;
  border: 1px solid transparent;
  color: var(--text-sub);
  font-family: var(--font-mono);
  font-size: 14px;
  display: grid;
  place-items: center;
  cursor: pointer;
  clip-path: var(--cut-sm);
}
.win-btn:hover { background: var(--bg-elev); border-color: var(--border); color: var(--text-main); }
.win-btn.is-close:hover { background: var(--danger); border-color: var(--danger); color: #fff; }

/* Hero Mascot Banner */
.hero {
  position: relative;
  height: 110px;
  min-height: 110px;
  margin: 8px 16px 0;
  background: #0d0a18;
  border: 1px solid var(--border-hi);
  clip-path: var(--cut-lg);
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 24px;
  overflow: hidden;
  flex-shrink: 0;
}
.hero-art {
  position: absolute;
  top: 0;
  right: 0;
  width: 65%;
  height: 100%;
  object-fit: cover;
  object-position: right 18%;
  pointer-events: none;
  z-index: 0;
}
.hero-shade {
  position: absolute;
  inset: 0;
  background: linear-gradient(90deg, #0d0a18 0%, #0d0a18 38%, rgba(13, 10, 24, 0.72) 62%, rgba(13, 10, 24, 0.08) 100%);
  pointer-events: none;
  z-index: 1;
}
.hero-left {
  position: relative;
  z-index: 2;
  display: flex;
  flex-direction: column;
  gap: 4px;
  max-width: 50%;
}
.hero-title {
  margin: 0;
  font-family: var(--font-display);
  font-size: 20px;
  font-weight: 700;
  letter-spacing: 0.05em;
  text-transform: uppercase;
  color: var(--text-main);
}
.hero-sub {
  font-size: 12px;
  color: var(--text-sub);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.hero-drop {
  position: relative;
  z-index: 2;
  margin-right: 170px;
  padding: 10px 18px;
  background: rgba(17, 13, 27, 0.92);
  border: 1px dashed var(--cel-purple-hi);
  clip-path: var(--cut-md);
  display: flex;
  align-items: center;
  gap: 12px;
  cursor: pointer;
  flex-shrink: 0;
}
.hero-drop:hover { border-color: var(--cel-lavender); background: var(--bg-elev); }
.drop-tag {
  padding: 5px 10px;
  background: var(--cel-purple);
  color: #fff;
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  letter-spacing: 0.06em;
  clip-path: var(--cut-sm);
}
.drop-info { display: flex; flex-direction: column; }
.drop-main { font-family: var(--font-display); font-size: 13px; font-weight: 700; color: var(--text-main); }
.drop-ext  { font-size: 10px; color: var(--text-sub); font-family: var(--font-mono); }

/* Active Loadout Bar */
.loadout-bar {
  margin: 8px 16px 0;
  padding: 6px 14px;
  background: var(--bg-panel);
  border: 1px solid var(--border);
  clip-path: var(--cut-md);
  display: flex;
  align-items: center;
  gap: 10px;
  height: 44px;
  min-height: 44px;
  flex-shrink: 0;
}
.loadout-label {
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  letter-spacing: 0.08em;
  color: var(--cel-lavender);
  text-transform: uppercase;
  white-space: nowrap;
  padding-right: 10px;
  border-right: 1px solid var(--border);
}
.loadout-chips {
  flex: 1;
  display: flex;
  align-items: center;
  gap: 8px;
  overflow-x: auto;
}
.loadout-chips::-webkit-scrollbar { height: 4px; }
.loadout-empty {
  font-family: var(--font-mono);
  font-size: 11px;
  color: var(--text-muted);
}
.skin-chip {
  display: flex;
  align-items: center;
  gap: 8px;
  background: #1d1536;
  border: 1px solid var(--cel-purple-hi);
  padding: 3px 8px 3px 6px;
  clip-path: var(--cut-sm);
  flex-shrink: 0;
}
.chip-ico {
  width: 22px;
  height: 22px;
  object-fit: cover;
  clip-path: var(--cut-sm);
  background: var(--bg-void);
}
.chip-text {
  display: flex;
  flex-direction: column;
  line-height: 1.1;
  cursor: pointer;
}
.chip-champ {
  font-family: var(--font-mono);
  font-size: 9px;
  font-weight: 700;
  color: var(--cel-lavender);
  text-transform: uppercase;
}
.chip-skin {
  font-size: 11px;
  font-weight: 600;
  color: var(--text-main);
  max-width: 140px;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.chip-remove {
  width: 18px;
  height: 18px;
  background: rgba(255,255,255,0.06);
  border: 0;
  color: var(--text-sub);
  font-family: var(--font-mono);
  font-size: 12px;
  cursor: pointer;
  display: grid;
  place-items: center;
  clip-path: var(--cut-sm);
}
.chip-remove:hover { background: var(--danger); color: #fff; }
.loadout-clear {
  height: 24px;
  padding: 0 10px;
  background: var(--bg-card);
  border: 1px solid var(--border);
  color: var(--text-sub);
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  text-transform: uppercase;
  cursor: pointer;
  clip-path: var(--cut-sm);
  flex-shrink: 0;
}
.loadout-clear:hover { border-color: var(--danger); color: var(--danger); }

/* Workspace */
.workspace {
  flex: 1;
  min-height: 0;
  display: flex;
  flex-direction: column;
  padding: 8px 16px;
  overflow: hidden;
}
.filter-bar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 8px;
  gap: 12px;
  flex-shrink: 0;
}
.sec-label {
  font-family: var(--font-display);
  font-size: 12px;
  font-weight: 700;
  letter-spacing: 0.08em;
  color: var(--cel-lavender);
  text-transform: uppercase;
}
.search-box {
  width: 260px;
  height: 28px;
  background: var(--bg-panel);
  border: 1px solid var(--border);
  padding: 0 12px;
  color: var(--text-main);
  font-size: 12px;
  outline: none;
  clip-path: var(--cut-sm);
}
.search-box:focus { border-color: var(--cel-purple-hi); }

/* Installed Mods Grid */
.mods-grid {
  flex: 1;
  min-height: 0;
  overflow-y: auto;
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(290px, 1fr));
  gap: 12px;
  align-content: start;
}
.mod-card {
  background: var(--bg-card);
  border: 1px solid var(--border);
  padding: 14px 16px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  clip-path: var(--cut-md);
}
.mod-card:hover { background: var(--bg-elev); }
.mod-card.is-enabled { border-color: var(--cel-purple-hi); background: #1d1633; }
.mod-left { display: flex; align-items: center; gap: 12px; min-width: 0; }
.mod-avatar {
  width: 42px;
  height: 42px;
  background: var(--bg-panel);
  border: 1px solid var(--border-hi);
  clip-path: var(--cut-sm);
  object-fit: cover;
  flex-shrink: 0;
}
.mod-meta { display: flex; flex-direction: column; min-width: 0; }
.mod-name {
  font-size: 13px;
  font-weight: 700;
  color: var(--text-main);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}
.mod-sub { font-family: var(--font-mono); font-size: 11px; color: var(--text-sub); margin-top: 2px; }
.mod-toggle {
  height: 28px;
  padding: 0 12px;
  background: var(--bg-panel);
  border: 1px solid var(--border-hi);
  color: var(--text-sub);
  font-family: var(--font-mono);
  font-size: 10px;
  font-weight: 700;
  text-transform: uppercase;
  cursor: pointer;
  clip-path: var(--cut-sm);
  flex-shrink: 0;
}
.mod-toggle.is-enabled { background: var(--cel-purple); border-color: var(--cel-purple-hi); color: #fff; }

/* Champion & Skin Explorer */
.explorer {
  flex: 1;
  min-height: 0;
  display: grid;
  grid-template-columns: 220px 1fr;
  gap: 12px;
  overflow: hidden;
}
.champ-sidebar {
  background: var(--bg-panel);
  border: 1px solid var(--border);
  clip-path: var(--cut-md);
  display: flex;
  flex-direction: column;
  min-height: 0;
  overflow: hidden;
}
.champ-list {
  flex: 1;
  min-height: 0;
  overflow-y: auto;
  padding: 6px;
  display: flex;
  flex-direction: column;
  gap: 3px;
}
.champ-item {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 6px 8px;
  background: transparent;
  border: 1px solid transparent;
  color: var(--text-sub);
  font-size: 12px;
  font-weight: 600;
  text-align: left;
  cursor: pointer;
  clip-path: var(--cut-sm);
  flex-shrink: 0;
}
.champ-item:hover { background: var(--bg-card); color: var(--text-main); }
.champ-item.is-selected { background: var(--cel-purple); color: #fff; }
.champ-item.has-active-mod { border-color: var(--cel-purple-hi); }
.champ-ico { width: 28px; height: 28px; clip-path: var(--cut-sm); object-fit: cover; background: var(--bg-void); flex-shrink: 0; }

.skin-stage {
  background: var(--bg-panel);
  border: 1px solid var(--border);
  clip-path: var(--cut-md);
  display: flex;
  flex-direction: column;
  min-height: 0;
  overflow: hidden;
}
.stage-banner {
  position: relative;
  height: 195px;
  min-height: 195px;
  background: #06050a;
  display: flex;
  align-items: flex-end;
  justify-content: space-between;
  padding: 16px 20px;
  overflow: hidden;
  flex-shrink: 0;
  border-bottom: 1px solid var(--border);
}
.stage-splash {
  position: absolute;
  inset: 0;
  width: 100%;
  height: 100%;
  object-fit: cover;
  object-position: center 20%;
  opacity: 0.82;
}
.stage-info {
  position: relative;
  z-index: 2;
  background: rgba(11, 9, 20, 0.9);
  border: 1px solid var(--border-hi);
  padding: 8px 14px;
  clip-path: var(--cut-md);
}
.stage-skin-name {
  font-family: var(--font-display);
  font-size: 18px;
  font-weight: 700;
  color: var(--text-main);
}
.stage-champ-sub {
  font-family: var(--font-mono);
  font-size: 10px;
  color: var(--cel-lavender);
  text-transform: uppercase;
}
.stage-activate {
  position: relative;
  z-index: 2;
  height: 38px;
  padding: 0 22px;
  background: var(--cel-purple);
  border: 1px solid var(--cel-purple-hi);
  color: #fff;
  font-family: var(--font-display);
  font-size: 13px;
  font-weight: 700;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  cursor: pointer;
  clip-path: var(--cut-md);
}
.stage-activate:hover { background: var(--cel-purple-hi); }
.stage-activate.is-ok { background: var(--success); border-color: var(--success); color: #050508; }

.chroma-bar {
  display: flex;
  align-items: center;
  gap: 10px;
  padding: 8px 18px;
  background: #090710;
  border-bottom: 1px solid var(--border);
  overflow-x: auto;
  flex-shrink: 0;
}
.chroma-btn {
  width: 28px;
  height: 28px;
  border-radius: 50%;
  border: 2px solid var(--border);
  background: var(--bg-elev);
  cursor: pointer;
  padding: 0;
  flex-shrink: 0;
  overflow: hidden;
}
.chroma-btn:hover { border-color: var(--text-main); }
.chroma-btn.is-active { border-color: var(--cel-purple-hi); transform: scale(1.12); }
.chroma-img { width: 100%; height: 100%; object-fit: cover; pointer-events: none; }

.skins-grid {
  flex: 1;
  min-height: 0;
  overflow-y: auto;
  padding: 12px;
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(134px, 1fr));
  grid-auto-rows: 174px;
  gap: 12px;
  align-content: start;
  background: var(--bg-void);
}
.skin-tile {
  height: 174px;
  min-height: 174px;
  background: var(--bg-card);
  border: 1px solid var(--border);
  padding: 0;
  display: flex;
  flex-direction: column;
  cursor: pointer;
  clip-path: var(--cut-sm);
  overflow: hidden;
  text-align: center;
  flex-shrink: 0;
}
.skin-tile:hover { border-color: var(--border-hi); }
.skin-tile.is-active { border-color: var(--cel-purple-hi); background: var(--bg-elev); }
.skin-thumb {
  width: 100%;
  height: 140px;
  min-height: 140px;
  flex-shrink: 0;
  object-fit: cover;
  object-position: top center;
  background: #07060c;
  display: block;
}
.skin-label {
  height: 32px;
  padding: 0 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 11px;
  font-weight: 600;
  color: var(--text-main);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
  flex-shrink: 0;
}

.drop-overlay {
  position: fixed;
  inset: 12px;
  background: rgba(11, 9, 20, 0.94);
  border: 2px dashed var(--cel-purple-hi);
  clip-path: var(--cut-lg);
  z-index: 9999;
  display: grid;
  place-items: center;
  pointer-events: none;
}
.drop-box { text-align: center; font-family: var(--font-mono); }
.drop-box h2 { font-size: 22px; color: var(--cel-lavender); margin: 0 0 8px; text-transform: uppercase; }
.drop-box p  { color: var(--text-sub); font-size: 13px; margin: 0; }

.footer {
  height: 26px;
  min-height: 26px;
  background: var(--bg-panel);
  border-top: 1px solid var(--border);
  padding: 0 16px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-family: var(--font-mono);
  font-size: 10px;
  color: var(--text-sub);
  flex-shrink: 0;
}
.footer-status { color: var(--cel-lavender); }
</style>
</head>
<body>
<div class="app" id="app-shell">
  <header class="titlebar">
    <div class="brand">
      <img src="http://tauri.localhost/celestiadiscord.webp" class="brand-avatar" alt="">
      <span class="brand-name">Celestial Skin Changer</span>
      <span class="brand-tag" id="patch-pill">IDLE</span>
    </div>

    <nav class="nav-tabs no-drag">
      <button class="nav-btn is-active" id="tab-champions" type="button">CHAMPIONS</button>
      <button class="nav-btn" id="tab-library" type="button">MY SKINS (<span id="mod-count">0</span>)</button>
    </nav>

    <div class="tb-actions no-drag">
      <button class="btn-cyber btn-sync" id="btn-sync-repo" type="button" title="Download the entire LeagueSkins repository for instant offline skin activation">☁ SYNC ALL REPO</button>
      <button class="btn-cyber" id="btn-import" type="button">+ IMPORT FILE</button>
      <button class="btn-cyber btn-primary" id="btn-patcher" type="button">START PATCHER</button>

      <div class="win-ctrls">
        <button class="win-btn" id="win-min" type="button" title="Minimize">&#8211;</button>
        <button class="win-btn" id="win-max" type="button" title="Maximize">&#9633;</button>
        <button class="win-btn is-close" id="win-close" type="button" title="Close Celestial">&times;</button>
      </div>
    </div>
  </header>

  <section class="hero">
    <img src="http://tauri.localhost/celestiadiscord.webp" class="hero-art" alt="">
    <div class="hero-shade"></div>

    <div class="hero-left">
      <h1 class="hero-title">Celestial Skin Changer</h1>
      <span class="hero-sub" id="hero-status">Select skins across multiple champions. Activating a skin automatically restarts the patcher with your loadout.</span>
    </div>

    <div class="hero-drop" id="drop-trigger">
      <span class="drop-tag">DROP / BROWSE</span>
      <div class="drop-info">
        <span class="drop-main">Import Local Skin</span>
        <span class="drop-ext">.fantome • .modpkg • .zip • .wad.client</span>
      </div>
    </div>
  </section>

  <section class="loadout-bar">
    <span class="loadout-label">ACTIVE LOADOUT (<span id="loadout-count">0</span>)</span>
    <div class="loadout-chips" id="loadout-chips">
      <span class="loadout-empty">No skins active — Pick skins from different champions below</span>
    </div>
    <button class="loadout-clear" id="btn-clear-loadout" type="button" hidden>CLEAR ALL</button>
  </section>

  <main class="workspace">
    <div class="filter-bar">
      <span class="sec-label" id="view-title">CHAMPION SKIN EXPLORER</span>
      <input class="search-box" id="search-input" type="search" placeholder="Search skins or champions..." autocomplete="off">
    </div>

    <div class="mods-grid" id="view-library" hidden></div>

    <div class="explorer" id="view-champions">
      <aside class="champ-sidebar">
        <div class="champ-list" id="champ-list"></div>
      </aside>
      <section class="skin-stage">
        <div class="stage-banner">
          <img class="stage-splash" id="stage-splash" alt="">
          <div class="stage-info">
            <div class="stage-champ-sub" id="stage-champ">CHAMPION</div>
            <div class="stage-skin-name" id="stage-skin">Select a Skin</div>
          </div>
          <button class="stage-activate" id="btn-activate-skin" type="button">ACTIVATE SKIN</button>
        </div>

        <div class="chroma-bar" id="chroma-bar" hidden></div>
        <div class="skins-grid" id="skins-grid"></div>
      </section>
    </div>
  </main>

  <div class="drop-overlay" id="drop-overlay" hidden>
    <div class="drop-box">
      <h2>DROP SKIN FILE TO IMPORT</h2>
      <p>Supports .fantome, .modpkg, .zip, and .wad.client</p>
    </div>
  </div>

  <footer class="footer">
    <span class="footer-status" id="status-line">READY</span>
    <span id="dd-version">CELESTIAL SKIN CHANGER</span>
  </footer>
</div>

<script>
(function () {
  const API_PORT = window.__CEL_API_PORT__ || 46321;
  const LOCAL_CACHE_BASE = `http://127.0.0.1:${API_PORT}/img?p=`;
  const D = { id: {}, portrait: {}, sk: {} };
  const CHAMPS = [];

  const GRID_NODE_CACHE = new Map();
  const $ = s => document.querySelector(s);
  const esc = t => String(t).replace(/[&<>"]/g, m => ({ '&':'&amp;', '<':'&lt;', '>':'&gt;', '"':'&quot;' }[m]));

  const getCdImg = path => {
    if (!path) return '';
    const rel = path.toLowerCase().replace('/lol-game-data/assets/', '');
    return LOCAL_CACHE_BASE + encodeURIComponent(rel);
  };

  const iconUrl = name => {
    if (D.portrait[name]) return getCdImg(D.portrait[name]);
    return LOCAL_CACHE_BASE + encodeURIComponent(`v1/champion-icons/${D.id[name] || -1}.png`);
  };

  function bindDiskCachedImage(imgEl, rawUrl, fallbackUrl) {
    const target = rawUrl || fallbackUrl || '';
    if (imgEl.getAttribute('src') !== target) {
      imgEl.src = target;
    }
  }

  const savedState = JSON.parse(localStorage.getItem('cel_ui_state') || '{}');
  const S = {
    tab: 'champions',
    q: '',
    mods: [],
    champIdx: savedState.champIdx || 0,
    skinIdx: savedState.skinIdx || 0,
    chromaIdx: savedState.chromaIdx ?? -1,
    patcherRunning: false
  };

  function saveUiState() {
    localStorage.setItem('cel_ui_state', JSON.stringify({
      champIdx: S.champIdx,
      skinIdx: S.skinIdx,
      chromaIdx: S.chromaIdx
    }));
  }

  function setStatus(msg) {
    const el = $('#status-line');
    if (el) el.textContent = String(msg).toUpperCase();
  }

  document.addEventListener('error', e => {
    if (e.target && e.target.tagName === 'IMG') {
      const fb = e.target.getAttribute('data-fb');
      if (fb && e.target.src !== fb) e.target.src = fb;
      else e.target.hidden = true;
    }
  }, true);

  function findInstalledModForSkin(champName, skinLabel, targetId) {
    const cLow = String(champName || '').toLowerCase();
    const sLow = String(skinLabel || '').toLowerCase();
    const idStr = String(targetId || '');
    return S.mods.find(m => {
      const mName = String(m.name || '').toLowerCase();
      const mChamp = String(m.champion || '').toLowerCase();
      if (idStr && mName.includes(idStr)) return true;
      if (mName === sLow) return true;
      if (mChamp === cLow && mName.includes(sLow)) return true;
      return false;
    });
  }

  function renderLoadoutBar() {
    const enabledMods = S.mods.filter(m => m.enabled);
    $('#loadout-count').textContent = enabledMods.length;
    const box = $('#loadout-chips');
    const clearBtn = $('#btn-clear-loadout');

    if (enabledMods.length === 0) {
      clearBtn.hidden = true;
      box.innerHTML = '<span class="loadout-empty">No skins active — Pick skins from different champions below</span>';
      return;
    }

    clearBtn.hidden = false;
    box.innerHTML = enabledMods.map(m => {
      const champ = m.champion || 'Custom';
      const ico = m.champion ? iconUrl(m.champion) : '';
      return `
        <div class="skin-chip">
          ${ico ? `<img class="chip-ico" src="${esc(ico)}" alt="">` : ''}
          <div class="chip-text" data-jump-champ="${esc(champ)}">
            <span class="chip-champ">${esc(champ)}</span>
            <span class="chip-skin">${esc(m.name || 'Skin')}</span>
          </div>
          <button class="chip-remove" data-remove-mod="${esc(m.id)}" type="button" title="Deactivate this skin">&times;</button>
        </div>
      `;
    }).join('');
  }

  async function restartPatcherWithLoadout() {
    if (!window.celApi) return;
    const btn = $('#btn-patcher');
    setStatus('Restarting patcher with new skin loadout...');
    btn.textContent = 'APPLYING...';
    await window.celApi.stop();
    await new Promise(r => setTimeout(r, 250));
    const hasEnabled = S.mods.some(m => m.enabled);
    if (hasEnabled) {
      await window.celApi.start();
      S.patcherRunning = true;
      btn.textContent = 'STOP PATCHER';
      btn.classList.add('is-running');
      $('#patch-pill').textContent = 'PATCHER ACTIVE';
      setStatus('Patcher restarted with ' + S.mods.filter(m => m.enabled).length + ' active skin(s)!');
    } else {
      S.patcherRunning = false;
      btn.textContent = 'START PATCHER';
      btn.classList.remove('is-running');
      $('#patch-pill').textContent = 'IDLE';
      setStatus('All skins disabled — Patcher stopped');
    }
  }

  async function refreshLibrary() {
    if (!window.celApi) return;
    const res = await window.celApi.invoke('get_mod_library', {});
    S.mods = (res && res.value && res.value.mods) ? res.value.mods : ((res && res.mods) || []);
    $('#mod-count').textContent = S.mods.length;
    renderLoadoutBar();
    renderLibrary();
    updateChampListBadges();
    if (S.tab === 'champions') updateStageInfo();
  }

  window.__CEL_ON_NATIVE_EVENT__ = async function(evType, msg) {
    setStatus(msg);
    $('#hero-status').textContent = msg;
    if (evType === 'SKIN_READY') {
      S.patcherRunning = true;
      $('#btn-patcher').textContent = 'STOP PATCHER';
      $('#btn-patcher').classList.add('is-running');
      $('#patch-pill').textContent = 'PATCHER ACTIVE';
      await refreshLibrary();
    } else if (evType === 'SYNC_PROGRESS') {
      const sb = $('#btn-sync-repo');
      if (sb) sb.textContent = msg;
    } else if (evType === 'SYNC_DONE') {
      const sb = $('#btn-sync-repo');
      if (sb) sb.textContent = '✓ REPO SYNCED';
    }
  };

  function renderLibrary() {
    const q = S.q.trim().toLowerCase();
    const box = $('#view-library');
    const filtered = S.mods.filter(m =>
      !q ||
      (m.name && m.name.toLowerCase().includes(q)) ||
      (m.champion && m.champion.toLowerCase().includes(q))
    );

    if (filtered.length === 0) {
      box.innerHTML = `<div class="mod-card"><div class="mod-meta"><span class="mod-name">No skins in library yet</span><span class="mod-sub">Select any skin in the Champions tab to activate</span></div></div>`;
      return;
    }

    box.innerHTML = filtered.map(m => {
      const champ = m.champion || 'Global';
      const ico = m.champion ? iconUrl(m.champion) : '';
      return `
        <div class="mod-card${m.enabled ? ' is-enabled' : ''}">
          <div class="mod-left">
            ${ico ? `<img class="mod-avatar" src="${esc(ico)}" alt="">` : `<div class="mod-avatar"></div>`}
            <div class="mod-meta">
              <span class="mod-name">${esc(m.name || 'Unnamed Skin')}</span>
              <span class="mod-sub">${esc(champ)} • ${esc(m.author || 'Custom')}</span>
            </div>
          </div>
          <button class="mod-toggle${m.enabled ? ' is-enabled' : ''}" data-mod-id="${esc(m.id)}" data-champ-name="${esc(m.champion || '')}" data-enabled="${m.enabled ? '1' : '0'}" type="button">
            ${m.enabled ? 'ENABLED' : 'OFF'}
          </button>
        </div>
      `;
    }).join('');
  }

  function renderChampList() {
    const q = S.q.trim().toLowerCase();
    const activeChamps = new Set(S.mods.filter(m => m.enabled && m.champion).map(m => m.champion.toLowerCase()));
    const listEl = $('#champ-list');
    listEl.innerHTML = '';
    const frag = document.createDocumentFragment();

    CHAMPS.forEach((n, i) => {
      if (q && !n.toLowerCase().includes(q)) return;
      const hasMod = activeChamps.has(n.toLowerCase());
      const btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'champ-item' + (i === S.champIdx ? ' is-selected' : '') + (hasMod ? ' has-active-mod' : '');
      btn.setAttribute('data-champ', String(i));

      const img = document.createElement('img');
      img.className = 'champ-ico';
      img.alt = '';
      img.loading = 'lazy';
      bindDiskCachedImage(img, iconUrl(n), '');

      const sp = document.createElement('span');
      sp.textContent = n;

      btn.appendChild(img);
      btn.appendChild(sp);
      frag.appendChild(btn);
    });
    listEl.appendChild(frag);
  }

  function updateChampListBadges() {
    const activeChamps = new Set(S.mods.filter(m => m.enabled && m.champion).map(m => m.champion.toLowerCase()));
    document.querySelectorAll('#champ-list .champ-item').forEach(btn => {
      const idx = +btn.getAttribute('data-champ');
      const name = CHAMPS[idx] || '';
      btn.classList.toggle('is-selected', idx === S.champIdx);
      btn.classList.toggle('has-active-mod', activeChamps.has(name.toLowerCase()));
    });
  }

  async function selectChamp(idx, keepSkinIdx) {
    if (idx < 0 || idx >= CHAMPS.length) idx = 0;
    const champChanged = (S.champIdx !== idx);
    S.champIdx = idx;
    if (!keepSkinIdx && champChanged) {
      S.skinIdx = 0;
      S.chromaIdx = -1;
    }
    saveUiState();
    updateChampListBadges();

    const name = CHAMPS[idx];
    const cid = D.id[name];

    if (cid && !D.sk[cid]) {
      try {
        const r = await fetch(`${LOCAL_CACHE_BASE}v1/champions/${cid}.json`);
        const j = await r.json();
        D.sk[cid] = j.skins || [];
      } catch (_) {
        D.sk[cid] = [{ id: cid * 1000, name: name, isBase: true }];
      }
    }
    renderSkinsForChamp(cid, name);
  }

  function updateStageInfo() {
    if (CHAMPS.length === 0) return;
    const name = CHAMPS[S.champIdx];
    const cid = D.id[name];
    const raw = D.sk[cid] || [{ id: cid * 1000, name: name, isBase: true }];
    if (S.skinIdx >= raw.length) S.skinIdx = 0;
    const picked = raw[S.skinIdx] || raw[0];

    const chromas = picked.chromas || [];
    const activeName = (S.chromaIdx === -1 || !chromas[S.chromaIdx]) ? picked.name : chromas[S.chromaIdx].name;
    const skinLabel = (picked.isBase && S.chromaIdx === -1) ? 'Default ' + name : activeName;
    const targetId = (S.chromaIdx === -1 || !chromas[S.chromaIdx]) ? picked.id : chromas[S.chromaIdx].id;

    $('#stage-champ').textContent = name;
    $('#stage-skin').textContent = skinLabel;

    const splashPath = picked.uncenteredSplashPath || picked.splashPath || picked.loadScreenPath || '';
    const splashEl = $('#stage-splash');
    splashEl.hidden = false;
    bindDiskCachedImage(splashEl, getCdImg(splashPath), '');

    const cBar = $('#chroma-bar');
    if (chromas.length > 0) {
      cBar.hidden = false;
      const baseIco = getCdImg(picked.chromaPath || picked.tilePath || picked.splashPath);
      cBar.innerHTML = '';

      const baseBtn = document.createElement('button');
      baseBtn.type = 'button';
      baseBtn.className = 'chroma-btn' + (S.chromaIdx === -1 ? ' is-active' : '');
      baseBtn.setAttribute('data-chroma', '-1');
      baseBtn.title = picked.name + ' (Base)';
      const bImg = document.createElement('img');
      bImg.className = 'chroma-img';
      bindDiskCachedImage(bImg, baseIco, '');
      baseBtn.appendChild(bImg);
      cBar.appendChild(baseBtn);

      chromas.forEach((ch, i) => {
        const cBtn = document.createElement('button');
        cBtn.type = 'button';
        cBtn.className = 'chroma-btn' + (S.chromaIdx === i ? ' is-active' : '');
        cBtn.setAttribute('data-chroma', String(i));
        cBtn.title = ch.name || ('Chroma ' + (i + 1));
        const cImg = document.createElement('img');
        cImg.className = 'chroma-img';
        bindDiskCachedImage(cImg, getCdImg(ch.chromaPath), '');
        cBtn.appendChild(cImg);
        cBar.appendChild(cBtn);
      });
    } else {
      cBar.hidden = true;
      cBar.innerHTML = '';
    }

    document.querySelectorAll('#skins-grid .skin-tile').forEach((tile, i) => {
      tile.classList.toggle('is-active', i === S.skinIdx);
    });

    const match = findInstalledModForSkin(name, skinLabel, targetId);
    const btn = $('#btn-activate-skin');
    if (picked.isBase && S.chromaIdx === -1) {
      btn.textContent = 'USE DEFAULT (CLEAR ' + name.toUpperCase() + ')';
      btn.className = 'stage-activate';
    } else if (match && match.enabled) {
      btn.textContent = '✓ ACTIVE IN LOADOUT';
      btn.className = 'stage-activate is-ok';
    } else {
      btn.textContent = 'ACTIVATE SKIN';
      btn.className = 'stage-activate';
    }
  }

  function renderSkinsForChamp(cid, name) {
    const grid = $('#skins-grid');
    if (grid.getAttribute('data-rendered-cid') === String(cid)) {
      updateStageInfo();
      return;
    }

    if (GRID_NODE_CACHE.has(cid)) {
      grid.innerHTML = '';
      grid.appendChild(GRID_NODE_CACHE.get(cid));
      grid.setAttribute('data-rendered-cid', String(cid));
      updateStageInfo();
      return;
    }

    const raw = D.sk[cid] || [{ id: cid * 1000, name: name, isBase: true }];
    const fallbackUrl = iconUrl(name);
    const container = document.createDocumentFragment();

    raw.forEach((sk, i) => {
      const thumbUrl = getCdImg(sk.tilePath || sk.loadScreenPath || sk.splashPath || sk.uncenteredSplashPath);
      const skinName = sk.isBase ? 'Default ' + name : sk.name;

      const btn = document.createElement('button');
      btn.type = 'button';
      btn.className = 'skin-tile' + (i === S.skinIdx ? ' is-active' : '');
      btn.setAttribute('data-skin', String(i));
      btn.title = skinName;

      const img = document.createElement('img');
      img.className = 'skin-thumb';
      img.setAttribute('data-fb', fallbackUrl);
      img.alt = '';
      bindDiskCachedImage(img, thumbUrl, fallbackUrl);

      const lbl = document.createElement('span');
      lbl.className = 'skin-label';
      lbl.textContent = skinName;

      btn.appendChild(img);
      btn.appendChild(lbl);
      container.appendChild(btn);
    });

    grid.innerHTML = '';
    grid.appendChild(container);
    grid.setAttribute('data-rendered-cid', String(cid));
    updateStageInfo();
  }

  function stashCurrentGridNodes() {
    const grid = $('#skins-grid');
    const curCid = Number(grid.getAttribute('data-rendered-cid'));
    if (curCid && grid.children.length > 0) {
      const frag = document.createDocumentFragment();
      while (grid.firstChild) frag.appendChild(grid.firstChild);
      GRID_NODE_CACHE.set(curCid, frag);
      grid.removeAttribute('data-rendered-cid');
    }
  }

  function switchTab(tab) {
    S.tab = tab;
    $('#tab-library').classList.toggle('is-active', tab === 'library');
    $('#tab-champions').classList.toggle('is-active', tab === 'champions');
    $('#view-library').hidden = tab !== 'library';
    $('#view-champions').hidden = tab !== 'champions';
    $('#view-title').textContent = tab === 'library' ? 'INSTALLED SKIN LIBRARY' : 'CHAMPION SKIN EXPLORER';
    if (tab === 'champions') selectChamp(S.champIdx, true);
    else renderLibrary();
  }

  async function importSkinPath(path) {
    if (!path || !window.celApi) return;
    setStatus('Importing ' + path + '...');
    await window.celApi.importSkin(path);
    await refreshLibrary();
    await restartPatcherWithLoadout();
  }
  window.__CEL_ON_FILE_DROPPED__ = importSkinPath;

  if (window.__TAURI__ && window.__TAURI__.event && window.__TAURI__.event.listen) {
    window.__TAURI__.event.listen('tauri://drag-enter', () => { $('#drop-overlay').hidden = false; });
    window.__TAURI__.event.listen('tauri://drag-leave', () => { $('#drop-overlay').hidden = true; });
    window.__TAURI__.event.listen('tauri://drag-drop', async ev => {
      $('#drop-overlay').hidden = true;
      const paths = (ev && ev.payload && ev.payload.paths) ? ev.payload.paths : [];
      for (const p of paths) await importSkinPath(p);
    });
  }

  window.addEventListener('dragover', e => { e.preventDefault(); $('#drop-overlay').hidden = false; });
  window.addEventListener('dragleave', e => { if (e.relatedTarget === null) $('#drop-overlay').hidden = true; });
  window.addEventListener('drop', async e => {
    e.preventDefault();
    $('#drop-overlay').hidden = true;
    const files = e.dataTransfer ? e.dataTransfer.files : [];
    for (let i = 0; i < files.length; i++) if (files[i].path) await importSkinPath(files[i].path);
  });

  $('#tab-library').addEventListener('click', () => switchTab('library'));
  $('#tab-champions').addEventListener('click', () => switchTab('champions'));
  $('#search-input').addEventListener('input', e => {
    S.q = e.target.value;
    if (S.tab === 'library') renderLibrary();
    else renderChampList();
  });

  const openImportDialog = () => {
    setStatus('Opening file browser...');
    window.__CEL_WIN_CMD__ = 'OPEN_FILE_DIALOG';
  };
  $('#btn-import').addEventListener('click', openImportDialog);
  $('#drop-trigger').addEventListener('click', openImportDialog);

  $('#btn-sync-repo').addEventListener('click', () => {
    $('#btn-sync-repo').textContent = '⏳ SYNCING REPO...';
    setStatus('Downloading full LeagueSkins repository in background...');
    window.__CEL_WIN_CMD__ = 'SYNC_ALL_REPO';
  });

  $('#btn-clear-loadout').addEventListener('click', async () => {
    if (!window.celApi) return;
    setStatus('Clearing all active skins...');
    for (const m of S.mods) {
      if (m.enabled) await window.celApi.toggle(m.id, false);
    }
    await refreshLibrary();
    await restartPatcherWithLoadout();
  });

  $('#btn-patcher').addEventListener('click', async () => {
    if (!window.celApi) return;
    const btn = $('#btn-patcher');
    if (!S.patcherRunning) {
      setStatus('Starting patcher...');
      btn.textContent = 'STARTING...';
      await window.celApi.start();
      S.patcherRunning = true;
      btn.textContent = 'STOP PATCHER';
      btn.classList.add('is-running');
      $('#patch-pill').textContent = 'PATCHER ACTIVE';
      setStatus('Patcher running — Ready for game');
    } else {
      setStatus('Stopping patcher...');
      await window.celApi.stop();
      S.patcherRunning = false;
      btn.textContent = 'START PATCHER';
      btn.classList.remove('is-running');
      $('#patch-pill').textContent = 'IDLE';
      setStatus('Patcher stopped');
    }
  });

  $('#win-min').addEventListener('click', () => { window.__CEL_WIN_CMD__ = 'MINIMIZE'; });
  $('#win-max').addEventListener('click', () => { window.__CEL_WIN_CMD__ = 'MAXIMIZE'; });
  $('#win-close').addEventListener('click', () => { window.__CEL_WIN_CMD__ = 'CLOSE'; });

  document.addEventListener('click', async e => {
    const rem = e.target.closest('[data-remove-mod]');
    if (rem && window.celApi) {
      const id = rem.getAttribute('data-remove-mod');
      await window.celApi.toggle(id, false);
      await refreshLibrary();
      await restartPatcherWithLoadout();
      return;
    }

    const jump = e.target.closest('[data-jump-champ]');
    if (jump) {
      const cName = jump.getAttribute('data-jump-champ').toLowerCase();
      const idx = CHAMPS.findIndex(c => c.toLowerCase() === cName);
      if (idx !== -1) {
        stashCurrentGridNodes();
        switchTab('champions');
        selectChamp(idx, false);
      }
      return;
    }

    const tog = e.target.closest('[data-mod-id]');
    if (tog && window.celApi) {
      const id = tog.getAttribute('data-mod-id');
      const champName = tog.getAttribute('data-champ-name');
      const cur = tog.getAttribute('data-enabled') === '1';

      if (!cur && champName) {
        for (const m of S.mods) {
          if (m.enabled && m.id !== id && m.champion && m.champion.toLowerCase() === champName.toLowerCase()) {
            await window.celApi.toggle(m.id, false);
          }
        }
      }
      await window.celApi.toggle(id, !cur);
      await refreshLibrary();
      await restartPatcherWithLoadout();
      return;
    }

    const chBtn = e.target.closest('[data-chroma]');
    if (chBtn) {
      S.chromaIdx = +chBtn.getAttribute('data-chroma');
      saveUiState();
      updateStageInfo();
      return;
    }

    const ch = e.target.closest('[data-champ]');
    if (ch) {
      const nextIdx = +ch.getAttribute('data-champ');
      if (nextIdx !== S.champIdx) {
        stashCurrentGridNodes();
        selectChamp(nextIdx, false);
      }
      return;
    }

    const sk = e.target.closest('[data-skin]');
    if (sk) {
      S.skinIdx = +sk.getAttribute('data-skin');
      S.chromaIdx = -1;
      saveUiState();
      updateStageInfo();
      return;
    }
  });

  $('#btn-activate-skin').addEventListener('click', async () => {
    if (CHAMPS.length === 0) return;
    const champName = CHAMPS[S.champIdx];
    const cid = D.id[champName];
    const raw = D.sk[cid] || [];
    const picked = raw[S.skinIdx];
    if (!picked) return;

    const chromas = picked.chromas || [];
    const chromaObj = (S.chromaIdx !== -1 && chromas[S.chromaIdx]) ? chromas[S.chromaIdx] : null;
    const baseSkinId = picked.id;
    const chromaId = chromaObj ? chromaObj.id : -1;
    const targetId = chromaId !== -1 ? chromaId : baseSkinId;
    const activeName = chromaObj ? chromaObj.name : picked.name;
    const skinLabel = (picked.isBase && chromaId === -1) ? 'Default ' + champName : activeName;

    if (picked.isBase && chromaId === -1) {
      for (const m of S.mods) {
        if (m.enabled && m.champion && m.champion.toLowerCase() === champName.toLowerCase()) {
          await window.celApi.toggle(m.id, false);
        }
      }
      await refreshLibrary();
      await restartPatcherWithLoadout();
      return;
    }

    const match = findInstalledModForSkin(champName, skinLabel, targetId);
    const btn = $('#btn-activate-skin');

    if (match && window.celApi) {
      btn.textContent = '⏳ APPLYING LOADOUT...';
      for (const m of S.mods) {
        if (m.enabled && m.id !== match.id && m.champion && m.champion.toLowerCase() === champName.toLowerCase()) {
          await window.celApi.toggle(m.id, false);
        }
      }
      await window.celApi.toggle(match.id, true);
      await refreshLibrary();
      await restartPatcherWithLoadout();
      return;
    }

    btn.textContent = '⏳ ACTIVATING SKIN...';
    window.__CEL_WIN_CMD__ = `DOWNLOAD_SKIN|${cid}|${baseSkinId}|${chromaId}|${champName}`;
  });

  refreshLibrary();

  (async () => {
    try {
      const res = await fetch(`${LOCAL_CACHE_BASE}v1/champion-summary.json`);
      const summary = await res.json();

      const seenChamps = new Set();
      const validChamps = summary.filter(c => {
        if (c.id <= 0 || seenChamps.has(c.name)) return false;
        seenChamps.add(c.name);
        return true;
      }).sort((a, b) => a.name.localeCompare(b.name));

      CHAMPS.length = 0;
      validChamps.forEach(c => {
        CHAMPS.push(c.name);
        D.id[c.name] = c.id;
        D.portrait[c.name] = c.squarePortraitPath;
      });
      renderChampList();
      selectChamp(S.champIdx, true);
    } catch (err) {
      setStatus('Failed to load champion manifest');
    }
  })();
})();
</script>
</body>
</html>
)CEL_UI_EOF";
