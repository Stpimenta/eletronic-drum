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
body{margin:0;font-family:Arial;background:#111;color:#eee}
header{padding:12px;text-align:center;background:#1c1c1c;font-weight:bold}
.container{padding:12px;max-width:480px;margin:auto}

.pad{
  background:#1e1e1e;
  border-radius:8px;
  padding:12px;
  margin-bottom:12px;
  position:relative;
}
.pad h3{margin:0 0 8px 0;font-size:16px}
label{font-size:13px;display:block;margin-top:10px}
.row{display:flex;align-items:center;gap:6px}

button{
  background:#333;
  color:#fff;
  border:none;
  border-radius:4px;
}
.btn-step{
  width:32px;
  height:32px;
  font-size:18px;
}
.btn-step:active{background:#555}

.btn-save{
  position:absolute;
  top:10px;
  right:10px;
  padding:4px 8px;
  font-size:11px;
  background:#2e7d32;
}
.btn-save:active{background:#388e3c}

input[type=number]{
  width:70px;
  height:32px;
  text-align:center;
  background:#111;
  color:#fff;
  border:1px solid #444;
  border-radius:4px;
}

footer{text-align:center;font-size:11px;color:#777;margin-top:16px}
</style>
</head>

<body>
<header>DRUM CONFIG</header>

<div class="container" id="pads"></div>

<footer>ESP01 Drum Interface</footer>

<script>
function step(id, delta){
  var el = document.getElementById(id);
  if(!el) return;
  var v = parseInt(el.value) || 0;
  el.value = v + delta;
  el.blur();
}

function sendPad(id){
  const commands = [];
  const t  = document.getElementById('p'+id+'t').value;
  const g  = document.getElementById('p'+id+'g').value;
  const n  = document.getElementById('p'+id+'n').value;
  const ph = document.getElementById('p'+id+'ph').value;
  const rt = document.getElementById('p'+id+'rt').value;

  commands.push(`SET PAD ${id} THRESHOLD ${t}`);
  commands.push(`SET PAD ${id} SENSITIVITY ${g}`);
  commands.push(`SET PAD ${id} NOTE ${n}`);
  commands.push(`SET PAD ${id} PEAK_HOLD ${ph}`);
  commands.push(`SET PAD ${id} RETRIGGER ${rt}`);

  // envia cada comando separadamente
  commands.forEach(cmd => {
    fetch("/cmd", {
      method: "POST",
      headers: { "Content-Type": "text/plain" },
      body: cmd + "\\n"
    }).catch(e => console.log("Erro ao enviar comando:", e));
  });
}

function renderPad(pad){
  return `
  <div class="pad">
    <h3>Pad ${pad.id} - ${pad.name}</h3>

    <button class="btn-save" onclick="sendPad(${pad.id})">Salvar</button>

    <label>Threshold
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}t', -1)">−</button>
        <input type="number" id="p${pad.id}t" value="${pad.threshold}">
        <button class="btn-step" onclick="step('p${pad.id}t', 1)">+</button>
      </div>
    </label>

    <label>Gain (x100)
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

    <label>Peak Hold
      <div class="row">
        <button class="btn-step" onclick="step('p${pad.id}ph', -10)">−</button>
        <input type="number" id="p${pad.id}ph" value="${pad.peak_hold}">
        <button class="btn-step" onclick="step('p${pad.id}ph', 10)">+</button>
      </div>
    </label>

    <label>Retrigger
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

      var container = document.getElementById("pads");
      container.innerHTML = "";

      data.pads.forEach(pad => {
        container.innerHTML += renderPad(pad);
      });
    })
    .catch(() => console.log("Falha ao carregar estado"));
}

window.onload = loadState;
</script>

</body>
</html>
)rawliteral";
