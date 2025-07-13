#include <WiFi.h>
#include <WebServer.h>
#include <LiquidCrystal.h>

// --- Konfigurasi Wi-Fi ---
const char* ssid = "Kost Putra Bu Asmi";
const char* password = "klarissa88";

// --- Konfigurasi LCD ---
LiquidCrystal lcd(21, 22, 19, 18, 5, 17);

// --- Konfigurasi Motor (gunakan pin sesuai wiring) ---
const int IN1 = 32;
const int IN2 = 33;
const int IN3 = 25;
const int IN4 = 26;

// --- Web Server ---
WebServer server(80);
String lastCommand = "";

// --- Fungsi Gerakan Motor ---
void stopMotor() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void moveLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

// --- Halaman HTML ---
String htmlPage() {
  String html = "<!DOCTYPE html><html><head><title>Kontrol Robot</title></head><body>";
  html += "<h2>Kontrol Mobil Robot</h2>";
  html += "<form action='/move' method='GET'>";
  html += "<button name='dir' value='forward'>Maju</button> ";
  html += "<button name='dir' value='backward'>Mundur</button><br><br>";
  html += "<button name='dir' value='left'>Kiri</button> ";
  html += "<button name='dir' value='right'>Kanan</button><br><br>";
  html += "<button name='dir' value='stop'>Stop</button>";
  html += "</form>";
  html += "<p>Perintah terakhir: " + lastCommand + "</p>";
  html += "</body></html>";
  return html;
}

// --- Handler Web ---
void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void handleMove() {
  if (server.hasArg("dir")) {
    lastCommand = server.arg("dir");
    if (lastCommand == "forward") moveForward();
    else if (lastCommand == "backward") moveBackward();
    else if (lastCommand == "left") moveLeft();
    else if (lastCommand == "right") moveRight();
    else stopMotor();

    // LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gerak:");
    lcd.setCursor(0, 1);
    lcd.print(lastCommand);
  }
  server.send(200, "text/html", htmlPage());
}

void setup() {
  Serial.begin(115200);

  // --- Inisialisasi LCD ---
  lcd.begin(16, 2);
  lcd.print("Connecting WiFi");

  // --- Motor Pin ---
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotor();

  // --- WiFi ---
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  // --- Routing ---
  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
