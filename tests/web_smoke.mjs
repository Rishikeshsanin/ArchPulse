import fs from 'node:fs';

const required = ['index.html','style.css','app.js','worker.js','sw.js','manifest.webmanifest','vercel.json','icon.svg'];
for (const file of required) {
  if (!fs.existsSync(file)) throw new Error(`Missing required web asset: ${file}`);
}

const html = fs.readFileSync('index.html','utf8');
const app = fs.readFileSync('app.js','utf8');
const ids = new Set([...html.matchAll(/id="([^"]+)"/g)].map(m => m[1]));
const refs = new Set([
  ...[...app.matchAll(/\$\('#([^']+)'\)/g)].map(m => m[1]),
  ...[...app.matchAll(/\$\("#([^\"]+)"\)/g)].map(m => m[1]),
]);
const missing = [...refs].filter(id => !ids.has(id));
if (missing.length) throw new Error(`JavaScript references missing DOM IDs: ${missing.join(', ')}`);

if (!html.includes('src="/app.js"')) throw new Error('index.html does not load /app.js');
if (!html.includes('rel="manifest"')) throw new Error('PWA manifest link missing');
if (!app.includes("new Worker('/worker.js')")) throw new Error('Web Worker integration missing');
if (!app.includes("serviceWorker.register('/sw.js')")) throw new Error('Service worker registration missing');
if (!app.includes('navigator.gpu')) throw new Error('WebGPU capability path missing');

const manifest = JSON.parse(fs.readFileSync('manifest.webmanifest','utf8'));
if (manifest.display !== 'standalone' || !manifest.icons?.length) throw new Error('PWA manifest is incomplete');
JSON.parse(fs.readFileSync('vercel.json','utf8'));

const bannedClaims = ['cycle-accurate AMD', 'real hardware counters'];
for (const phrase of bannedClaims) {
  if (html.toLowerCase().includes(phrase.toLowerCase())) throw new Error(`Misleading performance claim detected: ${phrase}`);
}

console.log(`ArchPulse web smoke checks passed: ${ids.size} DOM IDs, ${refs.size} JS ID references, ${required.length} required assets.`);
