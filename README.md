# Execução TensorFlow Lite Micro em SoC LiteX (Colorlight i5)

## Descrição do Projeto
- Objetivo: Integrar um modelo TensorFlow Lite Micro (TFLM) em um SoC gerado pelo LiteX na FPGA Colorlight i5, e dirigir o estado dos LEDs conforme a saída do modelo.
- Componentes principais:
  - LiteX SoC, UART e CSR para controle de LEDs.
  - Biblioteca TFLM e firmware C++.
  - Firmware em `firmware/main.cc` que:
    - Inicializa UART e realiza um autoteste de LEDs.
    - Carrega o modelo `hello_world_int8`, configura o intérprete TFLM.
    - Executa inferência em loop e atualiza o padrão dos LEDs.
- Fluxo de execução:
  1. Programar o bitstream na FPGA (mapeando GPIO de LEDs).
  2. Enviar o firmware (`main.bin`) via `litex_term`.
  3. Observar padrão dos LEDs de acordo com a saída do modelo.

## Diagrama Conceitual (Blocos)
- CPU RISC-V (PicoRV32)
  - Executa firmware C++ e TFLM.
  - Acessa periféricos via CSR.
- Memória (SRAM/SDRAM)
  - Área de arena do TFLM e dados do firmware.
- Periféricos LiteX
  - `UART`: logs e console.
  - `GPIOOut (leds_ext)`: controle de 8 LEDs.
- TFLM Runtime
  - Modelo `hello_world_int8`.
  - `MicroInterpreter` + `OpResolver` (FullyConnected).
- Interconexão CSR
  - Mapeia registros `leds_out` para pinos físicos definidos na plataforma.

Representação em blocos:
```

```

## Pinos Utilizados (Colorlight i5)
- LEDs externos (8 bits) mapeados em `litex/colorlight_i5.py` como `leds_ext`:
  - `leds_ext[0]` → `P17`
  - `leds_ext[1]` → `P18`
  - `leds_ext[2]` → `N18`
  - `leds_ext[3]` → `L20`
  - `leds_ext[4]` → `L18`
  - `leds_ext[5]` → `G20`
  - `leds_ext[6]` → `M18`
  - `leds_ext[7]` → `N17`
- UART:
  - `serial_tx` → `J17`
  - `serial_rx` → `H18`
- Clock/Reset:
  - `clk25` → `P3`
  - `cpu_reset_n` → `K18`

Referência das constraints: `build/colorlight_i5/gateware/colorlight_i5.lpf`.

## Build e Execução

1. Clonar o repositório

```powershell
git clone https://github.com/heribertomonteiro/execucao-tensorflowlitemicro-em-soc-litex.git
```

2. Entrar na pasta do FPGA
```powershell
cd execucao-tensorflowlitemicro-em-soc-litex
```

3. Instalar o openFPGALoader:

```powershell
apt install openfpgaloader
```

4. Entrar no ambiente de desenvolvimento OSS-CAD-SUITE

```powershell
source ~/../oss-cad-suite/environment
```

5. Gerar/compilar o SoC e o bitstream (diretório raiz do projeto):

```powershell
python3 litex/colorlight_i5.py --board i9 --revision 7.2 --build --cpu-type=picorv32 --ecppack-compress
```

6. Programar a FPGA com o bitstream de configuração:

```powershell
sudo openFPGALoader -b colorlight-i5 build/colorlight_i5/gateware/colorlight_i5.bit
```

7. Compilar o TFK com  o toolchain correto:

```powershell
cd firmware/tflm
make \
  CXX=riscv-none-elf-g++ \
  AR=riscv-none-elf-ar \
  CXXFLAGS="-march=rv32i2p0_m -mabi=ilp32 -O2 -g -std=c++17 -fno-rtti -fno-exceptions -fno-threadsafe-statics -ffunction-sections -fdata-sections -Wall -Wno-unused-parameter -I. -I./third_party/flatbuffers/include -I./third_party/gemmlowp -I./third_party/ruy -DTF_LITE_STATIC_MEMORY -DTF_LITE_DISABLE_X86_NEON -DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK -DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS -DTF_LITE_USE_GLOBAL_MAX -DTF_LITE_USE_GLOBAL_MIN"
```

8. Compilar o firmware (gera `main.bin`):

```powershell
cd .. 
make
```

9. Conectar via terminal serial (litex_term) e carregar o kernel (`main.bin`):

```powershell
cd build
sudo ~/../oss-cad-suite/bin/litex_term /dev/ttyACM0 --kernel main.bin
```

10. Se depois de rodar o comando acima e não aperecer nada, aperta ENTER e após aparecer **litex>** ou **RUNTIME>** é preciso digitar **reboot** e apertar enter.
