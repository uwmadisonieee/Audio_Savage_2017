# Audio Savage
This project is still in the works

## Setup
- `cd` into the `Pi` directory
- run `make`
- run `./Audio_Savage`
- go to `localhost:8000/index.html` 
  - if ssh into a machine go to `<SSH_IP_ADDRESS>:8000/index.html`
- Press any keyboard key on webpage and it print outs the keycode


```
i=0; while [ $i -le 100000 ]; do if [ $(( ($i * 2 * 2 / 4) % 10000)) -eq 0 ]; then echo $i; fi; i=`expr $i + 1`; done
```
