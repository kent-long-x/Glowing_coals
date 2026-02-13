/* Glowing_coals.ino - FastLED brazier coals */

#include <FastLED.h>

/* Behavior Config */
//#define USE_ALT_PALETTE   // uncomment to use alternate color palette (debug)
#define USE_THROB
#define USE_BLINK_INTERVAL

/* Hardware Config */
#define LED_PIN_NUM 6		// Arduino pin used for FastLED data
#define NUM_LEDS 	60		// number of LEDs in string

/* Power Save Timeout Config */
#define TIMEOUT_MIN 		180		// number of runtime minutes before power save
#define TIMEOUT_MS 			((long)TIMEOUT_MIN * 60 * 1000)
#define NUM_BLINKS 			4
#define BLINK_INTERVAL_SEC	20		// how often to blink, in seconds
#define BLINK_INTERVAL_MS	(BLINK_INTERVAL_SEC * 1000)
#define BLINK_DELAY_MS 		40
#define BLINK_COLOR_0		(CRGB::Green)
#define BLINK_COLOR_1		(CRGB::Blue)

#define LOOP_MS 			30 		// overall update interval, in ms
#define	OMIT_INTERVAL		5		// skip every n LEDs when updating, for smoothing

#define BRIGHT_OFFSET 10
#define BRIGHT_MIN 25
#define BRIGHT_MAX 100
#define BRIGHT_INIT (100)

#define THROB_A_BPM (20)		// global throb A rate
#define THROB_A_MIN (40)
#define THROB_A_VAL (40)
#define THROB_B_BPM (10)		// global throb B rate
#define THROB_B_MIN (40)
#define THROB_B_VAL (40)

#define BRIGHT_SCALE 20
#define PAL_IDX_SCALE 220

CRGB leds[NUM_LEDS];

DEFINE_GRADIENT_PALETTE(heatmap_gp){	// standard "heat map" for glowing colors
	0, 30, 0, 0,       //black
	180, 80, 2, 0,    //red
	220, 255, 70, 0,  //bright yellow
	255, 255, 150, 50
};

CRGBPalette16 heat_pal = heatmap_gp;


#ifdef USE_ALT_PALETTE
DEFINE_GRADIENT_PALETTE(altmap_gp){		// alternate color palette (debug)
	0, 0, 0, 0,         //black
	170, 25, 0, 30,     //blue
	210, 10, 150, 150,  //
	255, 80, 200, 40
};

CRGBPalette16 alt_pal = altmap_gp;
#endif

CRGBPalette16 *pal_ptr;			// addr of current palette

uint16_t bright_scale = BRIGHT_SCALE;
uint16_t pal_idx_scale = PAL_IDX_SCALE;

void setup() {

	FastLED.addLeds<WS2812B, LED_PIN_NUM, GRB>(leds, NUM_LEDS);
	FastLED.setBrightness(BRIGHT_MAX);
	
#ifdef USE_ALT_PALETTE
	pal_ptr = &alt_pal;			// alternate palette (debug)
#else
	pal_ptr = &heat_pal;		// standard palette
#endif
}

void loop() {
	static int count = 0;
	static int pal_idx = 0;
	CRGB new_vals[NUM_LEDS];

	if (millis() < TIMEOUT_MS) {		// check total run time for timeout

		/*** Normal coal display ***/

		EVERY_N_MILLISECONDS(LOOP_MS) {

			for (int i = 0; i < NUM_LEDS; i++) {
				uint8_t brightness = 0;
				uint8_t min_brightness;
				uint8_t pal_id;
				uint8_t	omit_count = 0;

				/* Set brightness */
				min_brightness =  (brightness - (brightness / 16));  // min = 7/8 of prev
				if (min_brightness < BRIGHT_MIN) {
					min_brightness = BRIGHT_MIN;
				}
					
				brightness = inoise8(i * bright_scale, millis() / 5, millis() & 0xFFFF);
				if (brightness < min_brightness) {		
					brightness = min_brightness;
				}

				/* Set color */
				pal_idx = inoise8(i * pal_idx_scale, millis() / 10);

#ifdef USE_THROB
				uint8_t throb_a = beatsin8(THROB_A_BPM, THROB_A_MIN, THROB_A_VAL, 0, 0);
				uint8_t throb_b = beatsin8(THROB_B_BPM, THROB_B_MIN, THROB_B_VAL, 0, 0);

				pal_idx += ((throb_a + throb_b) / 2) - ((THROB_A_VAL + THROB_A_VAL) / 4);
#endif

				if ((i + omit_count) % OMIT_INTERVAL != 0) {
					leds[i] = ColorFromPalette(*pal_ptr, pal_idx, (brightness + pal_idx / 4));
				}

				/* Update display */
				FastLED.show();

				if (++omit_count >= OMIT_INTERVAL) {
					omit_count = 0;
				}

			}  // end for (i < NUM_LEDS)
		} // end EVERY_N_MILLISECONDS(LOOP_MS) 

	} else {
		/*** Timeout - turn off LEDs, blink occasionally ***/

		EVERY_N_MILLISECONDS(BLINK_INTERVAL_MS) {
			for (int blink = 0; blink < NUM_BLINKS; blink++) {
				CRGB colors[2] = { BLINK_COLOR_0, BLINK_COLOR_1 };

#ifdef USE_BLINK_INTERVAL
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
		}  // end EVERY_N_MILLISECONDS(BLINK_INTERVAL_MS) 

	} // end if (millis() < TIMEOUT_MS) 

}
