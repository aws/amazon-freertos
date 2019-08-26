# Deep Learning Library

This is a lightweight deep neural network inference library. It contains operations and models for basic deep learning framework. With efficient implementation, many applications like face detecion and recognition can be deployed with a limited resource. Developpers can easily use the interface and the data structure to build their own applications.

## Feature
- Fully connection
- CNNs
- Pooling
- Activations
- Quantization
- Batch normalization
- Mobilenet
- Shufflenet

## API Guides

This library contains two types of matrix, float point and fixed point. All the operations do same logics for both types. 

### Data structure

- Float point matrix

This is a common type of matrix, its data is in float point format.
```c
typedef struct
{
    int w;          /*!< Width */
    int h;          /*!< Height */
    int c;          /*!< Channel */
    int n;          /*!< Number of filter, input and output must be 1 */
    int stride;     /*!< Step between lines */
    fptp_t *item;   /*!< Data */
} dl_matrix3d_t;
```

- Fixed point matrix

Quantized data has `exponent` variable besides others in float point matrix. Its data is in 16-bit fixed point format. The value is between -32768 and 32767.
```c
typedef struct
{
    /******* fix start *******/
    int w;          /*!< Width */
    int h;          /*!< Height */
    int c;          /*!< Channel */
    int n;          /*!< Number of filter, input and output must be 1 */
    int stride;     /*!< Step between lines */
    int exponent;   /*!< Exponent for quantization */
    qtp_t *item;    /*!< Data */
    /******* fix end *******/
} dl_matrix3dq_t;
```

- 8-bit image matrix

Different from quantized matrix, image data usually use 0-255 value in pixels. This contains an image matrix, with its original value in 8-bit.
```c
typedef struct
{
    int w;          /*!< Width */
    int h;          /*!< Height */
    int c;          /*!< Channel */
    int n;          /*!< Number of filter, input and output must be 1 */
    int stride;     /*!< Step between lines */
    uc_t *item;     /*!< Data */
} dl_matrix3du_t;
```

### Data layout

Data stroed in memory is in NHWC format, so the `stride` in the data structure above is `w * c`.

The provided coefficients are already transformed to this format.

### Matrix operation

#### Allocation and free

All matrix data start at allocation. According to the data type, 3 interfaces are offered:
```c
dl_matrix3d_t *dl_matrix3d_alloc(int n, int w, int h, int c);
dl_matrix3dq_t *dl_matrix3dq_alloc(int n, int w, int h, int c, int e);
dl_matrix3du_t *dl_matrix3du_alloc(int n, int w, int h, int c);
void dl_matrix3d_free(dl_matrix3d_t *m);
void dl_matrix3dq_free(dl_matrix3dq_t *m);
void dl_matrix3du_free(dl_matrix3du_t *m);
```

#### Transform between float point and fixed point

The two types of data can be transformed from each other. We can get float point matrix from a quantized matrix:
```c
dl_matrix3d_t *dl_matrix3d_from_matrixq(dl_matrix3dq_t *m);
```
Also, we can get fixed point matrix from a float point matrix:
```c
dl_matrix3dq_t *dl_matrixq_from_matrix3d(dl_matrix3d_t *m);
```
The exponent of the quantized matrix is auto ajusted according to the data distribution. We can also point a fixed value with the interface:
```c
dl_matrix3dq_t *dl_matrixq_from_matrix3d_qmf(dl_matrix3d_t *m, int exponent);
```
All the transformaion will allocate a new matrix to stroe the resulting data.

To change the exponent of a quantized matrix, we need to allocate a matrix and call the interface:
```c
void dl_matrix3dq_shift_exponent(dl_matrix3dq_t *out, dl_matrix3dq_t *in, int exponent);
```

#### Concatenation

We can concatenate 2, 4, 8 splited matrix into one, only in channel dimension.

#### Calculation

- Add

### Neural network

#### Fully connection

The size of input is (1, 1, 1, w), the size of filter is (1, h, w, 1).

#### Activation

- relu
- relu with clip
- leaky relu
- prelu

#### Pooling

- Global pooling

#### Convolution

- Fully convolution
- Depthwise convolution

#### Advanced network

- Mobilenet
- Mobilenet with shortcut
