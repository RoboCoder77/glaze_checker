#include <SoftwareSerial.h>

// Define SoftwareSerial RX and TX pins
#define SOFT_RX 12  // Change as needed
#define SOFT_TX 13  // Change as needed

SoftwareSerial softSerial(SOFT_RX, SOFT_TX);  // RX, TX
#define DOOR_LOCK 1
#define WATER_SOLENOID_VALVE 2
#define TORNADO_PUMP 3
#define ACTUATOR_FWD 4
#define ACTUATOR_REV 5
#define CHLORINE_PUMP 6
#define UV_LIGHT 7
#define RED_LIGHT 8
#define GREEN_LIGHT 9
#define DOOR_SENSOR 10
#define OUTLET_VALVE 11

#define TORNADO_TIME 2000
#define DRAIN_TIME 2000
#define CLEAN_TORNADO_TIME 3000
#define CLEAN_WATER_TIME 5000

bool istankFilled = false;
bool isDeglazed = false;
bool isCleaning = false;
bool cleaningCompleted = false;

bool isStartPressed;

int deglazingCycles = 3;   // Number of deglazing cycles
int cleaningCycles = 3;    // Number of cleaning cycles
int currentCleanCycle = 0;

void setup() {
    pinMode(DOOR_LOCK, OUTPUT);
    pinMode(WATER_SOLENOID_VALVE, OUTPUT);
    pinMode(TORNADO_PUMP, OUTPUT);
    pinMode(ACTUATOR_FWD, OUTPUT);
    pinMode(ACTUATOR_REV, OUTPUT);
    pinMode(CHLORINE_PUMP, OUTPUT);
    pinMode(UV_LIGHT, OUTPUT);
    pinMode(RED_LIGHT, OUTPUT);
    pinMode(GREEN_LIGHT, OUTPUT);
    pinMode(DOOR_SENSOR, INPUT);
    pinMode(OUTLET_VALVE, OUTPUT);
    Serial.begin(9600);
    softSerial.begin(9600);  // Software Serial

    softSerial.println("✅ Software Serial Ready!");
}

void loop() {
    checkSerialCommands();  // ✅ Always check for FILL_WATER updates

    if (!isDeglazed && isStartPressed) {
        startDeglazingProcess();
    }

    if (isDeglazed && isCleaning && !cleaningCompleted) {
        performCleaningCycle();
    }
}

// ✅ Function to check Serial for water status and commands
void checkSerialCommands() {
    while (softSerial.available()) {  // ✅ Process all available serial data
        String received = softSerial.readStringUntil('\n');
        received.trim();  // ✅ Remove extra spaces/newlines

        if (received == "FILL_WATER:1") {
            istankFilled = true;
            Serial.println("✅ Water filled, solenoid OFF.");
            digitalWrite(WATER_SOLENOID_VALVE, LOW);
        } else if (received == "FILL_WATER:0") {
            istankFilled = false;
            Serial.println("❌ Water low, solenoid ON.");
            digitalWrite(WATER_SOLENOID_VALVE, HIGH);
        }

        if (received == "START") {
            Serial.println("🚀 START Command Received. Waiting for water...");
            isStartPressed=true;
            digitalWrite(DOOR_LOCK, HIGH);
            Serial.println("🔒 DOOR LOCKED");
        }

        if (received == "CLEAN" && isDeglazed) {
            isCleaning = true;
            cleaningCompleted = false;
            currentCleanCycle = 0;
            Serial.println("🧼 CLEAN Command Received. Starting Cleaning Process...");
        }
    }
}


// ✅ Deglazing now ensures water is filled before each cycle EXCEPT the last one
void startDeglazingProcess() {
    Serial.println("======= Deglazing Started =======");

    for (int cycle = 1; cycle <= deglazingCycles; cycle++) {
        Serial.print("Deglazing Cycle: ");
        Serial.println(cycle);

        // ✅ Always wait for water before starting each cycle
        ensureWaterFilled(); 

        // ✅ Tornado Pump ON
        digitalWrite(TORNADO_PUMP, HIGH);
        Serial.println("TORNADO: ON");
        delay(TORNADO_TIME);
        digitalWrite(TORNADO_PUMP, LOW);
        Serial.println("TORNADO: OFF");

        // ✅ Drain Process (Actuator + Outlet Valve)
        Serial.println("Raising bucket");
        digitalWrite(ACTUATOR_FWD, HIGH);
        digitalWrite(ACTUATOR_REV, LOW);
        delay(DRAIN_TIME);
        digitalWrite(ACTUATOR_FWD, LOW);
        digitalWrite(ACTUATOR_REV, LOW);

        digitalWrite(OUTLET_VALVE, HIGH);
        Serial.println("Outlet valve open");
        delay(DRAIN_TIME);

        Serial.println("Lowering bucket");
        digitalWrite(ACTUATOR_REV, HIGH);
        digitalWrite(ACTUATOR_FWD, LOW);
        delay(DRAIN_TIME);
        digitalWrite(ACTUATOR_REV, LOW);
        digitalWrite(ACTUATOR_FWD, LOW);

        digitalWrite(OUTLET_VALVE, LOW);
        Serial.println("Outlet valve closed");

        // ✅ Only wait for `FILL_WATER:0` in the first two cycles
        if (cycle < deglazingCycles) {
            waitForWaterToBeLow();
        }
    }

    isDeglazed = true;
    Serial.println("======= Deglazing Completed =======");
    if(isDeglazed){
      softSerial.println("DEGLAZED:1");
      }
    
    
}






// ✅ Cleaning now ensures water is filled **at the start** before adding chlorine
void performCleaningCycle() {
    while (currentCleanCycle < cleaningCycles) {
        Serial.print("======= Cleaning Cycle ");
        Serial.print(currentCleanCycle + 1);
        Serial.println(" Started =======");

        // ✅ 1. Start Chlorine Pump and wait for FILL_WATER:1
        digitalWrite(CHLORINE_PUMP, HIGH);
        Serial.println("Chlorine Pump: ON (Waiting for FILL_WATER:1)");
        ensureWaterFilled();  // Wait until chlorine level is adequate
        digitalWrite(CHLORINE_PUMP, LOW);
        Serial.println("Chlorine Pump: OFF");

        // ✅ 2. Start Tornado & UV Light
        digitalWrite(TORNADO_PUMP, HIGH);
        digitalWrite(UV_LIGHT, HIGH);
        Serial.println("Tornado & UV Light: ON");
        delay(CLEAN_TORNADO_TIME);
        digitalWrite(TORNADO_PUMP, LOW);
        digitalWrite(UV_LIGHT, LOW);
        Serial.println("Tornado & UV Light: OFF");

        // ✅ 3. Open Outlet Valve to discharge
        digitalWrite(OUTLET_VALVE, HIGH);
        Serial.println("Outlet Valve: OPEN (Discharging)");
        delay(DRAIN_TIME);
        digitalWrite(OUTLET_VALVE, LOW);
        Serial.println("Outlet Valve: CLOSED");

        // ✅ 4. Wait for FILL_WATER:0 (Tank must be empty before refilling)
        waitForWaterToBeLow();

        // ✅ 5. Open Water Solenoid to refill
        digitalWrite(WATER_SOLENOID_VALVE, HIGH);
        Serial.println("Water Solenoid: ON (Refilling)");

        // ✅ 6. Wait for FILL_WATER:1 again
        ensureWaterFilled();
        digitalWrite(WATER_SOLENOID_VALVE, LOW);
        Serial.println("Water Solenoid: OFF");

        // ✅ 7. Open Outlet Valve again to discharge water
        digitalWrite(OUTLET_VALVE, HIGH);
        Serial.println("Outlet Valve: OPEN (Final Discharge)");
        delay(DRAIN_TIME);
        digitalWrite(OUTLET_VALVE, LOW);
        Serial.println("Outlet Valve: CLOSED");

        Serial.print("======= Cleaning Cycle ");
        Serial.print(currentCleanCycle + 1);
        Serial.println(" Completed =======");

        currentCleanCycle++;
    }

    Serial.println("======= Cleaning Process Completed =======");
    isCleaning = false;
    cleaningCompleted = true;
}


// ✅ Function to ensure tank is filled before moving forward
void ensureWaterFilled() {
    istankFilled = false;  // ✅ Force rechecking of water level
    Serial.println("⏳ Waiting for FILL_WATER:1 command...");

    while (!istankFilled) {  
        checkSerialCommands();  // ✅ Keep checking for new Serial input
        delay(500);
    }

    Serial.println("✅ Tank Filled. Proceeding...");
}



// ✅ Function to ensure tank is drained before waiting for refill
void waitForWaterToBeLow() {
    Serial.println("⏳ Waiting for FILL_WATER:0 command...");
    while (istankFilled) {
        checkSerialCommands();  // ✅ Continuously check Serial for FILL_WATER:0
        delay(500);
    }
    Serial.println("✅ Tank emptied. Waiting for next refill...");
}
