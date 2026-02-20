#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// 1. TENSOR TIPI TANIMI
typedef enum { TYPE_F32, TYPE_INT8 } TensorType;

// 2. UNION KULLANIMI (Bellek Tasarrufu)
// Aynı bellek alanı hem float hem int8 olarak kullanılabilir.
typedef union {
    float f32;
    int8_t i8;
} Element;

typedef struct {
    Element* data;      // Veri dizisi
    TensorType type;    // Mevcut tip
    uint32_t size;      // Eleman sayısı
    float scale;        // Quantization Ölçeği
    int8_t zero_point;  // Quantization Sıfır Noktası
} Tensor;

// Tensör Oluşturma
Tensor* create_tensor(uint32_t size, TensorType type) {
    Tensor* t = (Tensor*)malloc(sizeof(Tensor));
    t->size = size;
    t->type = type;
    t->data = (Element*)malloc(size * sizeof(Element));
    return t;
}

// QUANTIZATION: Float -> Int8 (Sıkıştırma)
void quantize(Tensor* t, float* raw_data) {
    float min = raw_data[0], max = raw_data[0];
    for(int i=0; i<t->size; i++) {
        if(raw_data[i] < min) min = raw_data[i];
        if(raw_data[i] > max) max = raw_data[i];
    }
    // Basit Scale/ZeroPoint hesabı
    t->scale = (max - min) / 255.0f;
    t->zero_point = (int8_t)(-min / t->scale - 128);
    t->type = TYPE_INT8;

    for(int i=0; i<t->size; i++) {
        t->data[i].i8 = (int8_t)(raw_data[i] / t->scale + t->zero_point);
    }
}

int main() {
    printf("--- TinyML Dinamik Tensor Demo ---\n");
    
    float example_data[4] = {10.5f, -2.0f, 5.75f, 0.0f};
    Tensor* my_tensor = create_tensor(4, TYPE_F32);

    printf("Original Float Data: ");
    for(int i=0; i<4; i++) printf("%.2f ", example_data[i]);
    
    // Sıkıştırma İşlemi
    quantize(my_tensor, example_data);
    
    printf("\nQuantized (INT8) Data in Memory: ");
    for(int i=0; i<4; i++) printf("%d ", my_tensor->data[i].i8);

    printf("\n\nBellek Tasarrufu: 4 Float (16 byte) -> 4 Int8 (4 byte) = %%75 Kazanc!\n");

    free(my_tensor->data);
    free(my_tensor);
    return 0;
}