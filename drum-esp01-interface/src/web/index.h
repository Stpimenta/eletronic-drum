#pragma once
#include <pgmspace.h>

const char PAGE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Drum Config</title>

<style>
:root {
  --bg: #111;
  --card: #1e1e1e;
  --line: #2a2a2a;
  --text: #eee;
  --muted: #999;
  --accent: #4caf50;
}

* { box-sizing: border-box; }

body {
  margin: 0;
  font-family: monospace;
  background: var(--bg);
  color: var(--text);
}

header {
  padding: 12px;
  text-align: center;
  background: #1c1c1c;
  font-weight: bold;
  border-bottom: 1px solid var(--line);
}

.container {
  display: flex;
  flex-wrap: wrap;
  gap: 12px;
  padding: 12px;
  max-width: 900px;
  margin: auto;
}

/* PAD */
.pad {
  background: var(--card);
  border: 1px solid var(--line);
  border-radius: 8px;
  padding: 12px;
  position: relative;
  width: 100%;
}

@media (min-width: 700px) {
  .pad {
    width: calc(50% - 6px);
  }
}

.pad h3 {
  margin: 0 0 8px 0;
  font-size: 14px;
  color: var(--accent);
}

.pad h3 span {
  color: var(--muted);
  font-size: 12px;
}

/* CAMPOS */
label {
  display: block;
  font-size: 12px;
  color: var(--muted);
  margin-top: 8px;
}

.row {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-top: 2px;
}

input[type=number] {
  width: 70px;
  height: 30px;
  background: #000;
  color: var(--text);
  border: 1px solid var(--line);
  border-radius: 4px;
  text-align: center;
  font-family: monospace;
}

button {
  background: #222;
  color: var(--text);
  border: 1px solid var(--line);
  border-radius: 4px;
  font-family: monospace;
  cursor: pointer;
}

button:active {
  background: #333;
}

.btn-step {
  width: 30px;
  height: 30px;
  font-size: 16px;
}

.btn-save {
  position: absolute;
  top: 10px;
  right: 10px;
  font-size: 11px;
  padding: 4px 8px;
  background: #1b5e20;
  border-color: #2e7d32;
}

footer {
  text-align: center;
  font-size: 10px;
  color: var(--muted);
  padding: 10px 0;
  border-top: 1px solid var(--line);
}
</style>
</head>

<body>

<header>ESP32 DRUM · CONFIG</header>

<div class="container" id="pads"></div>

<footer>UART Interface · ESP01</footer>

<script>
function step(id, delta){
  const el = document.getElementById(id);
  if(!el) return;
  el.value = (parseInt(el.value) || 0) + delta;
}

function sendPad(id){
  const cmds = [
    `SET PAD ${id} THRESHOLD ${document.getElementById('p'+id+'t').value}`,
    `SET PAD ${id} SENSITIVITY ${document.getElementById('p'+id+'g').value}`,
    `SET PAD ${id} NOTE ${document.getElementById('p'+id+'n').value}`,
    `SET PAD ${id} PEAK_HOLD ${document.getElementById('p'+id+'ph').value}`,
    `SET PAD ${id} RETRIGGER ${document.getElementById('p'+id+'rt').value}`,
  ];

  cmds.forEach(cmd => {
    fetch("/cmd", {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: cmd + "\\n"
    }).catch(() => {});
  });
}

function renderPad(pad){
  return `
  <div class="pad">
    <h3>Pad ${pad.id} <span>${pad.name}</span></h3>

    <button class="btn-save" onclick="sendPad(${pad.id})">Salvar</button>

    <label>Threshold
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}t', -1)">−</button>
        <input type="number" id="p${pad.id}t" value="${pad.threshold}">
        <button class="btn-step" onclick="step('p${pad.id}t', 1)">+</button>
      </div>
    </label>

    <label>Sensitivity
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}g', -1)">−</button>
        <input type="number" id="p${pad.id}g" value="${pad.sensitivity}">
        <button class="btn-step" onclick="step('p${pad.id}g', 1)">+</button>
      </div>
    </label>

    <label>Nota MIDI
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}n', -1)">−</button>
        <input type="number" id="p${pad.id}n" value="${pad.note}">
        <button class="btn-step" onclick="step('p${pad.id}n', 1)">+</button>
      </div>
    </label>

    <label>Peak Hold (µs)
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}ph', -10)">−</button>
        <input type="number" id="p${pad.id}ph" value="${pad.peak_hold}">
        <button class="btn-step" onclick="step('p${pad.id}ph', 10)">+</button>
      </div>
    </label>

    <label>Retrigger (µs)
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}rt', -100)">−</button>
        <input type="number" id="p${pad.id}rt" value="${pad.retrigger}">
        <button class="btn-step" onclick="step('p${pad.id}rt', 100)">+</button>
      </div>
    </label>
  </div>`;
}

function loadState(){
  fetch("/state")
    .then(r => r.json())
    .then(data => {
      if(!data || !data.pads) return;
      const c = document.getElementById("pads");
      c.innerHTML = "";
      data.pads.forEach(p => c.innerHTML += renderPad(p));
    });
}

window.onload = loadState;
</script>

</body>
</html>
)rawliteral";
