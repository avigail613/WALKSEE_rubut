import { useState, useEffect } from "react";
import { io } from "socket.io-client";

// כתובת השרת — שנה ל-IP של המחשב שלך
const SERVER = "http://localhost:3000";
const socket = io(SERVER);

export default function App() {
  const [location, setLocation]     = useState({ lat: 0, lon: 0 });
  const [emergency, setEmergency]   = useState(false);
  const [targetAddr, setTargetAddr] = useState("");
  const [isLoggedIn, setIsLoggedIn] = useState(false);
  const [password, setPassword]     = useState("");

  // סיסמה פשוטה לכניסה — שנה לסיסמה שלך
  const ADMIN_PASSWORD = "smart1234";

  // האזנה לעדכונים מהשרת
  useEffect(() => {
    // עדכון מיקום בזמן אמת
    socket.on("locationUpdate", (data) => {
      setLocation(data);
    });

    // קבלת התרעת חירום
    socket.on("emergency", () => {
      setEmergency(true);
    });

    return () => {
      socket.off("locationUpdate");
      socket.off("emergency");
    };
  }, []);

  // כניסה עם סיסמה
  function handleLogin() {
    if (password === ADMIN_PASSWORD) {
      setIsLoggedIn(true);
    } else {
      alert("סיסמה שגויה");
    }
  }

  // שליחת יעד חדש לשרת
  function sendTarget() {
    if (!targetAddr) return;

    // TODO: המרת כתובת לקואורדינטות דרך Google Geocoding
    // כרגע שולח כתובת טקסטואלית
    fetch(SERVER + "/setTarget", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ address: targetAddr })
    });
  }

  // מסך כניסה
  if (!isLoggedIn) {
    return (
      <div style={styles.center}>
        <h2>Smart Cane — כניסה</h2>
        <input
          type="password"
          placeholder="סיסמה"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          style={styles.input}
        />
        <button onClick={handleLogin} style={styles.button}>
          כניסה
        </button>
      </div>
    );
  }

  // קישור למפה עם המיקום הנוכחי
  const mapsUrl = `https://www.google.com/maps?q=${location.lat},${location.lon}`;

  return (
    <div style={styles.container}>
      <h2>Smart Cane — מעקב</h2>

      {/* התרעת חירום */}
      {emergency && (
        <div style={styles.emergency}>
          🚨 לחצן מצוקה נלחץ!
        </div>
      )}

      {/* מיקום נוכחי */}
      <div style={styles.card}>
        <h3>מיקום נוכחי</h3>
        <p>רוחב: {location.lat}</p>
        <p>אורך: {location.lon}</p>
        <a href={mapsUrl} target="_blank" rel="noreferrer">
          פתח במפה
        </a>
      </div>

      {/* הזנת יעד */}
      <div style={styles.card}>
        <h3>הגדרת יעד</h3>
        <input
          type="text"
          placeholder="הזן כתובת יעד"
          value={targetAddr}
          onChange={(e) => setTargetAddr(e.target.value)}
          style={styles.input}
        />
        <button onClick={sendTarget} style={styles.button}>
          שלח יעד
        </button>
      </div>
    </div>
  );
}

// עיצוב פשוט
const styles = {
  container: { padding: 20, fontFamily: "Arial", direction: "rtl" },
  center:    { display: "flex", flexDirection: "column", alignItems: "center", padding: 40 },
  card:      { border: "1px solid #ccc", borderRadius: 8, padding: 16, marginBottom: 16 },
  input:     { padding: 8, margin: 8, width: 200, fontSize: 16 },
  button:    { padding: "8px 16px", background: "#007bff", color: "white", border: "none", borderRadius: 4, cursor: "pointer" },
  emergency: { background: "red", color: "white", padding: 16, borderRadius: 8, marginBottom: 16, fontSize: 20 }
};
