#include "BMI088.h"
#include "rgb_lcd.h"

rgb_lcd lcd;

float ax = 0, ay = 0, az = 0;
BMI088 bmi088( BMI088_ACC_ADDRESS, BMI088_GYRO_ADDRESS );

void setup(void) 
{
    Wire.begin();
    Serial.begin(115200);

    lcd.begin(16, 2);
    lcd.setRGB(0, 0, 0);

    while (!Serial);
    Serial.println("BMI088 Raw Data");   

    while (1) {
        if (bmi088.isConnection()) {
            bmi088.initialize();
            Serial.println("BMI088 is connected");
            break;
        } else {
            Serial.println("BMI088 is not connected");
        }

        delay(2000);
    }
}

#define len 20
int i_x = 0;
float mem_x[len];
int i_y = 0;
float mem_y[len];
int i_z = 0;
float mem_z[len];

int delta = 30;
int max_intensity = 32000;
int min_intensity = 15000;
long double score = 0.f;

long long timer = 0;
int endtime = 15000;
long double maxScore = (endtime / delta) * 100.f;
long double min_acceptable_score = maxScore / 100;

void loop(void) 
{
    bmi088.getAcceleration(&ax, &ay, &az);

    // X direction
    mem_x[i_x] = ax;
    i_x = (i_x + 1) % len;

    // Find global maximum 
    float mn_x = 1e16, mx_x = -1e16;
    for (int i = 0; i < len; i++)
    {
        mn_x = min(mn_x, mem_x[i]);
        mx_x = max(mx_x, mem_x[i]);
    }

    // Y direction
    mem_y[i_y] = ay;
    i_y = (i_y + 1) % len;

    // Find global maximum 
    float mn_y = 1e16, mx_y = -1e16;
    for (int i = 0; i < len; i++)
    {
        mn_y = min(mn_y, mem_y[i]);
        mx_y = max(mx_y, mem_y[i]);
    }

    // Z direction
    mem_z[i_z] = az;
    i_z = (i_z + 1) % len;

    // Find global maximum 
    float mn_z = 1e16, mx_z = -1e16;
    for (int i = 0; i < len; i++)
    {
        mn_z = min(mn_z, mem_z[i]);
        mx_z = max(mx_z, mem_z[i]);
    }

    float mn = mn_x + mn_y + mn_z;
    float mx = mx_x + mx_y + mx_z;

    if (timer <= endtime)
    {
        if (mx - mn >= min_intensity)
        {
            lcd.setRGB(255, 0, 0);

            float meassured = mx - mn;
            if (meassured > max_intensity) {
              meassured = max_intensity;
            }
            score += ((meassured - min_intensity) / (max_intensity - min_intensity)) * 100;
        }
        else 
        {
            lcd.setRGB(0, 0, 0);
        }
        timer += delta;

        lcd.setCursor(0, 0);
        for (float i = 0; i < (float(timer) / float(endtime)); i += (float(endtime) / 16.f) / float(endtime)) {
          lcd.print("#");
        }

        lcd.setCursor(0, 1);
        lcd.print("Intensity: ");
        if (mx - mn > max_intensity) {
          lcd.print(100.f);
        } else {
          lcd.print(((mx - mn) / (max_intensity)) * 100);
        }
    }
    else 
    {
        lcd.clear();
        lcd.setRGB(0, 255, 0);
        lcd.setCursor(0, 0);
        lcd.print("Score: ");
        lcd.print(float((score / maxScore) * 100.f));
        lcd.setCursor(0, 1);
        if (score > min_acceptable_score) {
          lcd.print(" You may drink! ");
        } else {
          lcd.print("D: Try again!");
        }
    }

    delay(delta);
}