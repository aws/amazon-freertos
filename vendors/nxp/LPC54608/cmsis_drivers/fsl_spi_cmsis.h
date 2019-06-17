/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _FSL_SPI_CMSIS_H_
#define _FSL_SPI_CMSIS_H_

#include "fsl_spi.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#if defined(FSL_FEATURE_SOC_DMA_COUNT) && (FSL_FEATURE_SOC_DMA_COUNT)
#include "fsl_spi_dma.h"
#endif

#if defined(SPI0)
extern ARM_DRIVER_SPI SPI0_Interrupt_Driver;
#endif /* SPI0 */

#if defined(SPI1)
extern ARM_DRIVER_SPI SPI1_Interrupt_Driver;
#endif /* SPI1 */

#if defined(SPI2)
extern ARM_DRIVER_SPI SPI2_Interrupt_Driver;
#endif /* SPI2 */

#if defined(SPI3)
extern ARM_DRIVER_SPI SPI3_Interrupt_Driver;
#endif /* SPI3 */

#if defined(SPI4)
extern ARM_DRIVER_SPI SPI4_Interrupt_Driver;
#endif /* SPI4 */

#if defined(SPI5)
extern ARM_DRIVER_SPI SPI5_Interrupt_Driver;
#endif /* SPI5 */

#if defined(SPI6)
extern ARM_DRIVER_SPI SPI6_Interrupt_Driver;
#endif /* SPI6 */

#if defined(SPI7)
extern ARM_DRIVER_SPI SPI7_Interrupt_Driver;
#endif /* SPI7 */

#if defined(SPI8)
extern ARM_DRIVER_SPI SPI8_Interrupt_Driver;
#endif /* SPI8 */

#if defined(SPI9)
extern ARM_DRIVER_SPI SPI9_Interrupt_Driver;
#endif /* SPI9 */

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && (FSL_FEATURE_SOC_DMA_COUNT))

#if defined(SPI0)
extern ARM_DRIVER_SPI SPI0_Dma_Driver;
#endif /* SPI0 */

#if defined(SPI1)
extern ARM_DRIVER_SPI SPI1_Dma_Driver;
#endif /* SPI1 */

#if defined(SPI2)
extern ARM_DRIVER_SPI SPI2_Dma_Driver;
#endif /* SPI2 */

#if defined(SPI3)
extern ARM_DRIVER_SPI SPI3_Dma_Driver;
#endif /* SPI3 */

#if defined(SPI4)
extern ARM_DRIVER_SPI SPI4_Dma_Driver;
#endif /* SPI4 */

#if defined(SPI5)
extern ARM_DRIVER_SPI SPI5_Dma_Driver;
#endif /* SPI5 */

#if defined(SPI6)
extern ARM_DRIVER_SPI SPI6_Dma_Driver;
#endif /* SPI6 */

#if defined(SPI7)
extern ARM_DRIVER_SPI SPI7_Dma_Driver;
#endif /* SPI7 */

#if defined(SPI8)
extern ARM_DRIVER_SPI SPI8_Dma_Driver;
#endif /* SPI8 */

#if defined(SPI9)
extern ARM_DRIVER_SPI SPI9_Dma_Driver;
#endif /* SPI9 */

#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif
