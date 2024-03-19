#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "decompressor.h"
#include "compressor.h"

typedef int dataType; // Change this to your desired data type
void delay_ms(int ms) {
    k_msleep(ms);
}

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

int benchmark_compressor(uint8_t *uncompressed_data, size_t uncompressed_data_len)
{
	int res = 0;
	unsigned char *compressed_data = NULL;
	size_t compressed_len = 0;

	TampCompressor compressor;
	TampConf compressor_conf = {.literal = 8, .window = 10, .use_custom_dictionary = false};
	unsigned char *compressor_buffer = NULL;

	if (!(compressor_buffer = malloc(1 << compressor_conf.window)))
	{
		printk("OOM - Compressor (1)\n");
		return -1;
	}

	if (!(compressed_data = malloc(uncompressed_data_len)))
	{
		printk("OOM  - Compressor (2)\n");
		free(compressor_buffer);
		return -1;
	}

	if (TAMP_OK != tamp_compressor_init(&compressor, &compressor_conf, compressor_buffer))
	{
		printk("Failed to initialize compressor\n");
		free(compressor_buffer);
		free(compressed_data);
		return -1;
	}

	printk("Beginning compressing...\n");
	size_t consumed;
	if (TAMP_OK != tamp_compressor_compress_and_flush(
					   &compressor,
					   compressed_data,
					   uncompressed_data_len,
					   &compressed_len,
					   uncompressed_data,
					   uncompressed_data_len,
					   &consumed,
					   false))
	{
		printk("Failed to compress data\n");
		free(compressor_buffer);
		free(compressed_data);
		return -1;
	}

	printk("Consumed: %zu\n", consumed);
	printk("Compressed Length: %zu\n", compressed_len);
	// print compression ratio
	printk("Compression ratio : %f \n", (float)consumed / compressed_len);

	free(compressor_buffer);
	free(compressed_data);

	return res;
}

int benchmark_decompressor(uint8_t *compressed_data, size_t compressed_data_len)
{
	int res = 0;

	unsigned char *output = NULL, *window_buffer = NULL;
	size_t output_size = 100 << 20;
	size_t output_written_size;

	TampDecompressor decompressor;

	if (!(window_buffer = malloc(32 << 10)))
	{
		printk("OOM\n");
		return -1;
	}

	if (!(output = malloc(output_size)))
	{
		printk("OOM\n");
		free(window_buffer);
		return -1;
	}

	if (TAMP_OK != tamp_decompressor_init(&decompressor, NULL, window_buffer))
	{
		printk("Failed to initialize compressor\n");
		free(window_buffer);
		free(output);
		return -1;
	}

	if (0 > tamp_decompressor_decompress(
				&decompressor,
				output,
				output_size,
				&output_written_size,
				compressed_data,
				compressed_data_len,
				&compressed_data_len))
	{
		printk("Failed to decompress data\n");
		free(window_buffer);
		free(output);
		return -1;
	}

	printk("output_written_size: %zu\n", output_written_size);

	free(window_buffer);
	free(output);

	return res;
}

const float data[] = {
	9.739696383476257324e-01,
	9.132320880889892578e-01,
	8.655097484588623047e-01,
	8.232104182243347168e-01,
	7.462038993835449219e-01,
	6.420824527740478516e-01,
	5.477223396301269531e-01,
	4.262472987174987793e-01,
	3.253796100616455078e-01,
	2.798264622688293457e-01,
	2.288503199815750122e-01,
	1.366594433784484863e-01,
	4.772234335541725159e-02,
	1.084598712623119354e-02,
	1.008676812052726746e-01,
	1.887201666831970215e-01,
	2.277657240629196167e-01,
	2.407809048891067505e-01,
	2.570498883724212646e-01,
	2.635574936866760254e-01,
	2.939262390136718750e-01,
	3.188720047473907471e-01,
	3.492407798767089844e-01,
	4.023861289024353027e-01,
	4.392624795436859131e-01,
	4.501084685325622559e-01,
	4.620390534400939941e-01,
	4.750542342662811279e-01,
	4.891540110111236572e-01, 
	4.869848191738128662e-01, 
	5.075922012329101562e-01, 
	5.108459591865539551e-01, 
	5.303687453269958496e-01,
	5.433839559555053711e-01, 
	5.694143176078796387e-01, 
	5.824295282363891602e-01, 
	6.084598898887634277e-01, 
	6.193058490753173828e-01,
	6.420824527740478516e-01, 
	6.507592201232910156e-01, 
	6.583514213562011719e-01, 
	6.572667956352233887e-01, 
	6.550976037979125977e-01,
	6.268980503082275391e-01, 
	6.214750409126281738e-01, 
	6.019522547721862793e-01, 
	5.824295282363891602e-01, 
	5.477223396301269531e-01,
	5.227765440940856934e-01, 
	4.837310314178466797e-01, 
	4.707158207893371582e-01, 
	4.338394701480865479e-01, 
	4.175705015659332275e-01,
	4.034707248210906982e-01, 
	3.926247358322143555e-01, 
	3.828633427619934082e-01, 
	3.828633427619934082e-01, 
	3.796095550060272217e-01,
	3.839479386806488037e-01, 
	3.752711415290832520e-01, 
	3.687635660171508789e-01, 
	3.665943741798400879e-01, 
	3.676789700984954834e-01,
	3.644251525402069092e-01, 
	3.698481619358062744e-01, 
	3.709327578544616699e-01, 
	3.698481619358062744e-01, 
	3.644251525402069092e-01,
	3.752711415290832520e-01, 
	3.828633427619934082e-01, 
	3.872017264366149902e-01, 
	3.698481619358062744e-01, 
	3.861171305179595947e-01,
	3.763557374477386475e-01, 
	3.796095550060272217e-01, 
	3.817787468433380127e-01, 
	3.839479386806488037e-01, 
	3.752711415290832520e-01,
	3.817787468433380127e-01, 
	3.796095550060272217e-01, 
	3.731019496917724609e-01, 
	3.774403333663940430e-01, 
	3.861171305179595947e-01,
	3.785249590873718262e-01, 
	3.882863223552703857e-01, 
	3.774403333663940430e-01, 
	3.828633427619934082e-01, 
	3.991323113441467285e-01,
	3.969631195068359375e-01, 
	3.958785235881805420e-01, 
	4.305856823921203613e-01, 
	4.360086619853973389e-01, 
	4.479392766952514648e-01,
	4.544468522071838379e-01, 
	4.598698616027832031e-01, 
	4.522776603698730469e-01, 
	4.642082452774047852e-01, 
	4.663774371147155762e-01,
	4.587852358818054199e-01, 
	4.869848191738128662e-01, 
	7.809110879898071289e-01, 
	1.000000000000000000e+00, 
	9.598698616027832031e-01,
	9.045553207397460938e-01, 
	8.427332043647766113e-01, 
	7.787418365478515625e-01, 
	7.071583271026611328e-01, 
	6.312364339828491211e-01,
	5.260303616523742676e-01, 
	4.186550974845886230e-01, 
	3.145336210727691650e-01, 
	2.331887185573577881e-01, 
	2.049891501665115356e-01,
	1.420824229717254639e-01, 
	6.399132311344146729e-02, 
	0.000000000000000000e+00, 
	9.978307783603668213e-02, 
	1.876355707645416260e-01,
	2.386117130517959595e-01, 
	2.505423128604888916e-01, 
	2.678958773612976074e-01, 
	2.516269087791442871e-01, 
	0.000000000000000000e+00,
	9.739696383476257324e-01,
	9.132320880889892578e-01,
	8.655097484588623047e-01,
	8.232104182243347168e-01,
	7.462038993835449219e-01,
	6.420824527740478516e-01,
	5.477223396301269531e-01,
	4.262472987174987793e-01,
	3.253796100616455078e-01,
	2.798264622688293457e-01,
	2.288503199815750122e-01,
	1.366594433784484863e-01,
	4.772234335541725159e-02,
	1.084598712623119354e-02,
	1.008676812052726746e-01,
	1.887201666831970215e-01,
	2.277657240629196167e-01,
	2.407809048891067505e-01,
	2.570498883724212646e-01,
	2.635574936866760254e-01,
	2.939262390136718750e-01,
	3.188720047473907471e-01,
	3.492407798767089844e-01,
	4.023861289024353027e-01,
	4.392624795436859131e-01,
	4.501084685325622559e-01,
	4.620390534400939941e-01,
	4.750542342662811279e-01,
	4.891540110111236572e-01, 
	4.869848191738128662e-01, 
	5.075922012329101562e-01, 
	5.108459591865539551e-01, 
	5.303687453269958496e-01,
	5.433839559555053711e-01, 
	5.694143176078796387e-01,
	6.420824527740478516e-01,
	5.477223396301269531e-01,
	4.262472987174987793e-01,
	3.253796100616455078e-01,
	2.798264622688293457e-01,
	2.288503199815750122e-01,
	1.366594433784484863e-01,
	4.772234335541725159e-02,
	1.084598712623119354e-02,
	1.008676812052726746e-01,
	1.887201666831970215e-01,
	2.277657240629196167e-01,
	2.407809048891067505e-01,
	2.570498883724212646e-01,
	2.635574936866760254e-01,
	2.939262390136718750e-01,
	3.188720047473907471e-01,
	3.492407798767089844e-01,
	4.023861289024353027e-01,
	4.392624795436859131e-01,
	4.501084685325622559e-01,
	4.620390534400939941e-01,
	4.750542342662811279e-01,
	4.891540110111236572e-01, 
	4.869848191738128662e-01, 
	5.075922012329101562e-01, 
	5.108459591865539551e-01, 
	5.303687453269958496e-01,
	5.433839559555053711e-01, 
	5.694143176078796387e-01,
	5.433839559555053711e-01 
	};

int main(int argc, char *argv[])
{

	size_t data_len = sizeof(data);

	// --- Data pre-processing: starts ---

	// Find the total number of values in the data row
	uint32_t size_of_readings_array = sizeof(data) / sizeof(data[0]);
	printf("size_of_readings_array: %d\n", size_of_readings_array);

	int8_t *readings = (int8_t *)malloc(size_of_readings_array);
	if (readings == NULL)
	{
		printk("Error: malloc failed\n");
		return -1;
	}

	for (uint32_t i = 0; i < size_of_readings_array; i += 1)
	{
		// Check if the number is close to an integer
		if ((data[i] - (int8_t)data[i]) < 0.0001)
		{								   // Adjust tolerance as needed
			readings[i] = (int8_t)data[i]; // Simply cast to int
			printf("readings[i] = %d\n", readings[i]);
		}
		else
		{
			// Scale and round the number to fit within the range of integers
			printf("value = %f \n", data[i] * 100 + 0.5);
			readings[i] = (int8_t)(data[i] * 100 + 0.5); // Scale by 1000 and round
			printf("readings[i] = %d\n", readings[i]);
		}
	}

	//for (;;){
benchmark_compressor(readings, size_of_readings_array);
delay_ms(3000);
benchmark_compressor(readings, size_of_readings_array);
delay_ms(3000);
benchmark_compressor(readings, size_of_readings_array);
delay_ms(3000);
benchmark_compressor(readings, size_of_readings_array);
delay_ms(3000);
benchmark_compressor(readings, size_of_readings_array);
delay_ms(3000);
benchmark_compressor(readings, size_of_readings_array);
	 

	// benchmark_decompressor(data, data_len);

	return 0;
}