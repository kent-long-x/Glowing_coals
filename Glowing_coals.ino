/* Glowing_coals.ino - FastLED brazier coals */

#include <FastLED.h>


//#define USE_ALT_PALETTE   // XXX debug
#define USE_THROB
#define USE_TIMEOUT_BLINK

#define LED_PIN_NUM 6
#define NUM_LEDS 60
#define DELAY_MS 50
#define TIMEOUT_MIN 180
#define TIMEOUT_MS ((long)TIMEOUT_MIN * 60 * 1000)
#define NUM_BLINKS 2
#define TIMEOUT_BLINK_SEC 15
#define TIMEOUT_BLINK_MS (TIMEOUT_BLINK_SEC * 1000)
#define BLINK_DELAY_MS 20
#define LOOP_MS 40 
#define FADE_PERIOD 9
#define BRIGHT_MIN 2
#define BRIGHT_MAX 64
#define BRIGHT_INIT (100)
#define THROB_VAL (40)
#define THROB_A_BPM (20)
#define THROB_B_BPM (10)

#define BRIGHT_SCALE 20
#define PAL_IDX_SCALE 220
#define BLINK_COLOR_0 (CRGB::Green)
#define BLINK_COLOR_1 (CRGB::Blue)

CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE(heatmap_gp){
	0, 30, 0, 0,       //black
	180, 80, 2, 0,    //red
	220, 255, 70, 0,  //bright yellow
	255, 255, 150, 50
};

CRGBPalette16 heat_pal = heatmap_gp;


#ifdef USE_ALT_PALETTE
DEFINE_GRADIENT_PALETTE(altmap_gp){
	0, 0, 0, 0,         //black
	170, 25, 0, 30,     //blue
	210, 10, 150, 150,  //
	255, 80, 200, 40
};

CRGBPalette16 alt_pal = altmap_gp;
#endif

uint16_t bright_scale = BRIGHT_SCALE;
uint16_t pal_idx_scale = PAL_IDX_SCALE;

void setup() {
	// put your setup code here, to run once:
	FastLED.addLeds<WS2812B, LED_PIN_NUM, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(BRIGHT_MAX);
}

void loop() {
	// put your main code here, to run repeatedly:
	static int count = 0;
	static int pal_idx = 0;
	uint8_t throb_val = 20;
	CRGB new_vals[NUM_LEDS];

#if 1
	if (millis() < TIMEOUT_MS) {
		/* Normal coal display */
		EVERY_N_MILLISECONDS(LOOP_MS) {
			for (int i = 0; i < NUM_LEDS; i++) {
				uint8_t brightness = 0;
				uint8_t min_brightness;
				uint8_t pal_idx;

				min_brightness =  (brightness - (brightness / 4));  // min = 3/4 of prev
				brightness = inoise8(i * bright_scale, millis() / 5, millis() & 0xFFFF);
				if (brightness < min_brightness) {		
					brightness = min_brightness;
				}

				pal_idx = inoise8(i * pal_idx_scale, millis() / 10);

#ifdef USE_THROB
				uint8_t throb_a = beatsin8(THROB_A_BPM, BRIGHT_MIN, THROB_VAL, 0, 0);
				uint8_t throb_b = beatsin8(THROB_B_BPM, BRIGHT_MIN, THROB_VAL, 0, 0);

				pal_idx += ((throb_a + throb_b) / 2) - (THROB_VAL / 2);
#endif


#ifndef USE_ALT_PALETTE
				leds[i] = ColorFromPalette(heat_pal, pal_idx, (brightness + pal_idx / 4));
#else
				leds[i] = ColorFromPalette(alt_pal, pal_idx, (brightness + pal_idx / 4));
#endif

				/* Update display */
				FastLED.show();
			}
		}
#endif
	} else {
		/* Timeout - turn off LEDs, blink occasionally */

		EVERY_N_MILLISECONDS(TIMEOUT_BLINK_MS) {
			for (int blink = 0; blink < NUM_BLINKS; blink++) {
				CRGB colors[2] = { BLINK_COLOR_0, BLINK_COLOR_1 };

#ifdef USE_TIMEOUT_BLINK
				for (int i = 0; i < NUM_LEDS; i++) {
					if (blink & 1) {
						leds[i] = colors[1];
					} else {
						leds[i] = colors[0];
					}
				}
				FastLED.show();
				delay(BLINK_DELAY_MS);  // on time
#endif

				for (int i = 0; i < NUM_LEDS; i++) {
					leds[i] = CRGB::Black;
				}
				FastLED.show();
				delay(BLINK_DELAY_MS * 4);  // longer off time
			}
		}
	}



#if 0      
  // delay(DELAY_MS);

  count++;
  if (count >= NUM_LEDS) {
    count = 0;
  }


  /* Move dot */
    static int8_t   dot_num = 0;
    static bool     dot_dir_up = true;

    if (dot_dir_up) {
      dot_num++;
      if (dot_num == (NUM_LEDS-1)) {
        dot_dir_up = false;
      }
    } else {
      dot_num--;
      if (dot_num == 0) {
        dot_dir_up = true;
      }
    }

    leds[dot_num] = CRGB::DarkGrey;
#endif
}
