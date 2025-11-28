extern "C" {
	#include <generated/csr.h>
	#include <irq.h>
	#include <uart.h>
}

#include <math.h>

#include "tflm/tensorflow/lite/micro/micro_interpreter.h"
#include "tflm/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tflm/tensorflow/lite/schema/schema_generated.h"
#include "tflm/tensorflow/lite/micro/system_setup.h"
#include "tflm/examples/hello_world/models/hello_world_int8_model_data.h"

constexpr int kTensorArenaSize = 3024;
uint8_t tensor_arena[kTensorArenaSize];

extern "C" void isr(void) {}

void print_char(char c) {
	while(uart_txfull_read());
	uart_rxtx_write(c);
}

void print_str(const char* s){
	while(*s){
		print_char(*s++);
	}
}

void print_nl() {
	print_char('\n');
	print_char('\n');

}

void print_int(int val) {
	if (val < 0){
		print_char('-');
		val = -val;
	}

	if(val == 0){
		print_char('0');
		return;
	} 

	char buf[12];
	int i = 0;
	while (val > 0){
		buf[i++] = '0' + (val % 10);
		val /= 10;
	}
	while (i > 0) { print_char(buf[--i]); }
}

void print_float(float val, int decimals){
	if (val < 0){
		print_char('.');
		val = -val;
	}

	int ipart = (int)val;
	print_int(ipart);
	print_char('-');

	float fpart = val - ipart;
	for (int i = 0; i < decimals; i++){
		fpart *= 10;
		int digit = (int)fpart;
		print_char('0' + digit);
		fpart -= digit;
	}
}

#ifdef CSR_LEDS_OUT_ADDR
static inline void set_leds(uint8_t value){ leds_out_write((uint32_t)value); }
#else
static inline void set_leds(uint8_t){ /* no-op */ }
#endif

uint8_t sine_to_leds(float sine_value) {
	int num_leds = (int)((sine_value + 1.0f) * 4.0f + 0.5f);

	if(num_leds < 0){
		num_leds = 0;
	}
	if(num_leds > 8){
		num_leds = 8;
	}

	uint8_t pattern = 0;
	for (int i = 0; i < num_leds; i++){
		pattern |= (1 << i);
	}

	return pattern;
}

void print_leds(uint8_t pattern) {
	print_str(" [");
	for( int i = 7; i >= 0; i-- ){
		if(pattern & (1 << i)){
			print_char('*');
		} else {
			print_char('-');
		}
	}
	print_char(']');
}

int  main(void)
{

	uart_init();

	// Autoteste simples de LEDs (progressivo) para validar CSR/pinos
	for (int k = 0; k < 8; k++) {
		set_leds(1u << k);
		for (volatile int d = 0; d < 120000; d++);
	}
	set_leds(0);

	
	const tflite::Model* model = tflite::GetModel(g_hello_world_int8_model_data);
	if(model->version() != TFLITE_SCHEMA_VERSION){
		print_str("ERROR: Model version!");
		print_nl();
		while(1);
	}
	
	print_str("Creating resolver...");
	print_nl();

	tflite::MicroMutableOpResolver<1> resolver;
	if (resolver.AddFullyConnected() != kTfLiteOk){
		print_str("ERROR: AddFullyConnected");
		print_nl();
		while(1);
	}
	
	print_str("Creating interpreter...");
	print_nl();

	tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, kTensorArenaSize);

	print_str("Allocating tensors...");
	print_nl();

	
	if (interpreter.AllocateTensors() != kTfLiteOk){
		print_str("ERROR: AllocateTensors");
		print_nl();
		while(1);
	}

	print_str("Arena used:");
	print_int(interpreter.arena_used_bytes());
	print_str(" / ");
	print_int(kTensorArenaSize);
	print_nl();


	TfLiteTensor* input = interpreter.input(0);
	TfLiteTensor* output = interpreter.output(0);

	int count = 0;
	while(1){
		const float two_pi = 6.28318530718f;
		float x = (count * two_pi) / 200.0f; // 200 amostras por ciclo

		// Quantiza sin(x) de acordo com params do tensor de entrada
		float x_f = sinf(x);
		int8_t x_q = (int8_t)lrintf(x_f / input->params.scale + input->params.zero_point);
		input->data.int8[0] = x_q;

		if(interpreter.Invoke() != kTfLiteOk){
			print_str("ERROR: Invoke");
			print_nl();
			while(1);
		
		}

		int8_t y_q = output->data.int8[0];
		float y = (y_q - output->params.zero_point) * output->params.scale;
		float expected = sinf(x);

		uint8_t led_pattern = sine_to_leds(y);
		set_leds(led_pattern);
		
		
		for (volatile int i = 0; i < 25000; i++);

		count++;

		if(count >= 200){
			count = 0;
			print_str("----- Cycle Complete  -----");
			print_nl();
		}

	}

	return 0;
}