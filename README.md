```powershell
git clone https://github.com/heribertomonteiro/execucao-tensorflowlitemicro-em-soc-litex.git
```

2. Entrar na pasta do FPGA
```powershell
cd transmissao-de-dados-via-LoRa
cd fpga
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

7. Gerar `csr.h` (pelo build do LiteX) e compilar firmware (gera `main.bin`):

```powershell
cd firmware
make
```

8. Conectar via terminal serial (litex_term) e carregar o kernel (`main.bin`):

```powershell
sudo ~/../oss-cad-suite/bin/litex_term /dev/ttyACM0 --kernel main.bin
```

9. Se depois de rodar o comando acima e não aperecer nada, aperta ENTER e após aparecer **litex>** ou **RUNTIME>** é preciso digitar **reboot** e apertar enter.
