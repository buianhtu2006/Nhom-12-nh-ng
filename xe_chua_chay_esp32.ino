/*
  Xe chữa cháy mini - firmware tham khảo cho ESP32 DevKit.

  Điều khiển qua Serial Monitor (115200 baud, gửi "No line ending"):
    F: tiến 1 giây; B: lùi 1 giây; L/R: quay 1 giây; S: dừng tất cả;
    P: bật bơm tối đa 3 giây.

  Sơ đồ chân chỉ là cấu hình tham khảo. Đối chiếu bo mạch/module thực tế
  trước khi cấp nguồn. Không nối động cơ hoặc bơm trực tiếp vào GPIO.
*/

const uint8_t MOTOR_LEFT_IN1 = 25;
const uint8_t MOTOR_LEFT_IN2 = 26;
const uint8_t MOTOR_RIGHT_IN1 = 16;
const uint8_t MOTOR_RIGHT_IN2 = 17;
const uint8_t PUMP_PIN = 27;

const uint8_t FLAME_LEFT_PIN = 32;
const uint8_t FLAME_CENTER_PIN = 33;
const uint8_t FLAME_RIGHT_PIN = 34;
const bool FLAME_ACTIVE_LOW = true;

const uint8_t ULTRASONIC_TRIG_PIN = 18;
const uint8_t ULTRASONIC_ECHO_PIN = 19;

const unsigned long MOTOR_COMMAND_TIMEOUT_MS = 1000;
const unsigned long PUMP_MAX_RUNTIME_MS = 3000;
const unsigned long DISTANCE_INTERVAL_MS = 100;
const unsigned long STATUS_INTERVAL_MS = 1000;
const unsigned int OBSTACLE_DISTANCE_CM = 20;

unsigned long motionStartedAt = 0;
unsigned long pumpStartedAt = 0;
unsigned long lastDistanceReadAt = 0;
unsigned long lastStatusAt = 0;
unsigned int distanceCm = 0;
bool motorsRunning = false;
bool pumpRunning = false;
bool obstacleStopReported = false;

bool flameIsActive(uint8_t pin) {
  const int value = digitalRead(pin);
  return FLAME_ACTIVE_LOW ? value == LOW : value == HIGH;
}

bool flameDetected() {
  return flameIsActive(FLAME_LEFT_PIN) ||
         flameIsActive(FLAME_CENTER_PIN) ||
         flameIsActive(FLAME_RIGHT_PIN);
}

void stopMotors() {
  digitalWrite(MOTOR_LEFT_IN1, LOW);
  digitalWrite(MOTOR_LEFT_IN2, LOW);
  digitalWrite(MOTOR_RIGHT_IN1, LOW);
  digitalWrite(MOTOR_RIGHT_IN2, LOW);
  motorsRunning = false;
}

void stopPump() {
  digitalWrite(PUMP_PIN, LOW);
  pumpRunning = false;
}

void stopAll() {
  stopMotors();
  stopPump();
}

void setMotion(bool leftForward, bool rightForward) {
  digitalWrite(MOTOR_LEFT_IN1, leftForward ? HIGH : LOW);
  digitalWrite(MOTOR_LEFT_IN2, leftForward ? LOW : HIGH);
  digitalWrite(MOTOR_RIGHT_IN1, rightForward ? HIGH : LOW);
  digitalWrite(MOTOR_RIGHT_IN2, rightForward ? LOW : HIGH);
  motorsRunning = true;
  motionStartedAt = millis();
  obstacleStopReported = false;
}

unsigned int readDistanceCm() {
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  const unsigned long duration =
      pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 20000UL);
  if (duration == 0) {
    return 0;
  }
  return static_cast<unsigned int>(duration / 58UL);
}

void handleCommand(char command) {
  switch (command) {
    case 'F':
      setMotion(true, true);
      Serial.println("Lenh: tien");
      break;
    case 'B':
      setMotion(false, false);
      Serial.println("Lenh: lui");
      break;
    case 'L':
      setMotion(false, true);
      Serial.println("Lenh: re trai");
      break;
    case 'R':
      setMotion(true, false);
      Serial.println("Lenh: re phai");
      break;
    case 'S':
      stopAll();
      Serial.println("Da dung xe va tat bom");
      break;
    case 'P':
      if (!pumpRunning) {
        digitalWrite(PUMP_PIN, HIGH);
        pumpRunning = true;
        pumpStartedAt = millis();
        Serial.println("Bom bat; tu ngat sau toi da 3 giay");
      }
      break;
    default:
      Serial.println("Lenh khong hop le; dung F/B/L/R/S/P");
      break;
  }
}

void setup() {
  pinMode(MOTOR_LEFT_IN1, OUTPUT);
  pinMode(MOTOR_LEFT_IN2, OUTPUT);
  pinMode(MOTOR_RIGHT_IN1, OUTPUT);
  pinMode(MOTOR_RIGHT_IN2, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(FLAME_LEFT_PIN, INPUT);
  pinMode(FLAME_CENTER_PIN, INPUT);
  pinMode(FLAME_RIGHT_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  stopAll();
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  Serial.begin(115200);
  Serial.println("ESP32 san sang. Gui F/B/L/R/S/P qua Serial Monitor.");
}

void loop() {
  while (Serial.available() > 0) {
    char command = static_cast<char>(Serial.read());
    if (command >= 'a' && command <= 'z') {
      command = static_cast<char>(command - 'a' + 'A');
    }
    if (command != '\r' && command != '\n' && command != ' ') {
      handleCommand(command);
    }
  }

  const unsigned long now = millis();

  if (now - lastDistanceReadAt >= DISTANCE_INTERVAL_MS) {
    lastDistanceReadAt = now;
    distanceCm = readDistanceCm();
  }

  if (motorsRunning && now - motionStartedAt >= MOTOR_COMMAND_TIMEOUT_MS) {
    stopMotors();
    Serial.println("Timeout: da dung dong co");
  }

  if (motorsRunning &&
      (distanceCm == 0 || distanceCm <= OBSTACLE_DISTANCE_CM)) {
    stopMotors();
    if (!obstacleStopReported) {
      Serial.println("Canh bao: vat can gan, da dung dong co");
      obstacleStopReported = true;
    }
  }

  if (pumpRunning && now - pumpStartedAt >= PUMP_MAX_RUNTIME_MS) {
    stopPump();
    Serial.println("Timeout: da tat bom");
  }

  if (now - lastStatusAt >= STATUS_INTERVAL_MS) {
    lastStatusAt = now;
    Serial.print("Lua: ");
    Serial.print(flameDetected() ? "PHAT HIEN" : "khong phat hien");
    Serial.print(" | Khoang cach (cm): ");
    if (distanceCm == 0) {
      Serial.println("khong co echo");
    } else {
      Serial.println(distanceCm);
    }
  }
}
