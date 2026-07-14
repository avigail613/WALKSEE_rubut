// ============================================================
//  server.js — שרת Node.js
//  מקבל מיקום מ-ESP32, שולח לדפדפן בזמן אמת
//  כתובת: http://localhost:3000
// ============================================================

const express    = require('express');
const http       = require('http');
const { Server } = require('socket.io');
const cors       = require('cors');
const nodemailer = require('nodemailer');

const app    = express();
const server = http.createServer(app);

// socket.io = תקשורת בזמן אמת עם הדפדפן
const io = new Server(server, {
  cors: { origin: '*' }
});

app.use(cors());
app.use(express.json());

// ==================== משתנים ====================

// מיקום אחרון שהגיע מ-ESP32
let lastLocation = { lat: 0, lon: 0, time: null };

// מידע על יעד נוכחי
let currentTarget = { lat: 0, lon: 0, address: '' };

// ==================== נתיבי API ====================

// ESP32 שולח מיקום כל כמה שניות
// POST /location { lat, lon }
app.post('/location', (req, res) => {
  const { lat, lon } = req.body;

  lastLocation = { lat, lon, time: new Date() };

  // שולח לכל הדפדפנים המחוברים בזמן אמת
  io.emit('locationUpdate', lastLocation);

  res.json({ ok: true });
});

// דפדפן שואל את המיקום האחרון
// GET /location
app.get('/location', (req, res) => {
  res.json(lastLocation);
});

// ESP32 שולח חירום
// POST /emergency { lat, lon, email }
app.post('/emergency', async (req, res) => {
  const { lat, lon, email } = req.body;

  // שליחת מייל
  await sendEmergencyMail(lat, lon, email);

  // התרעה לדפדפן
  io.emit('emergency', { lat, lon, time: new Date() });

  res.json({ ok: true });
});

// דפדפן שולח יעד כשהמערכת לא פועלת
// POST /setTarget { lat, lon, address }
app.post('/setTarget', (req, res) => {
  currentTarget = req.body;

  // שולח ל-ESP32 דרך WebSocket
  io.emit('newTarget', currentTarget);

  res.json({ ok: true });
});

// ==================== שליחת מייל חירום ====================

async function sendEmergencyMail(lat, lon, email) {
  // nodemailer = ספרייה לשליחת מיילים
  const transporter = nodemailer.createTransporter({
    service: 'gmail',
    auth: {
      user: 'your_email@gmail.com',  // שנה לאימייל שלך
      pass: 'your_app_password'       // סיסמת אפליקציה של Gmail
    }
  });

  const mapsLink = `https://www.google.com/maps?q=${lat},${lon}`;

  await transporter.sendMail({
    from:    'your_email@gmail.com',
    to:      email,
    subject: '🚨 חירום — Smart Cane',
    text:    `לחצן מצוקה נלחץ!\nמיקום: ${mapsLink}`
  });

  console.log('[Server] מייל חירום נשלח ל-' + email);
}

// ==================== WebSocket ====================

// כשדפדפן מתחבר
io.on('connection', (socket) => {
  console.log('[Server] דפדפן התחבר');

  // שולח מיקום אחרון מייד
  socket.emit('locationUpdate', lastLocation);

  socket.on('disconnect', () => {
    console.log('[Server] דפדפן התנתק');
  });
});

// ==================== הפעלה ====================

const PORT = 3000;
server.listen(PORT, () => {
  console.log(`[Server] רץ על http://localhost:${PORT}`);
});
