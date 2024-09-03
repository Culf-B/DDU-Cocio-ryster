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

long long timer = 0;

void loop(void) 
{
    bmi088.getAcceleration(&ax, &ay, &az);

    mem_x[i_x] = ax;
    i_x = (i_x + 1) % len;

    // Find global maximum 
    float mn = 1e16, mx = -1e16;
    for (int i = 0; i < len; i++)
    {
        mn = min(mn, mem_x[i]);
        mx = max(mx, mem_x[i]);
    }

    if (timer <= 10000)
    {
        lcd.setCursor(0, 0);
        if (mx - mn >= 4400.0f)
        {
            lcd.setRGB(255, 0, 0);
            timer += 25;
        }
        else 
        {
            lcd.setRGB(0, 0, 0);
        }

        for (int i = 200; i <= timer; i += 620)
            lcd.print("#");
    }
    else 
    {
        lcd.setRGB(0, 255, 0);
        lcd.setCursor(0, 1);
        lcd.print(" You may drink! ");
    }

    delay(25);
}