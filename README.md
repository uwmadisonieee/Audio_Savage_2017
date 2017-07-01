# Audio Savage
This project is still in the works

## Setup
- `cd` into the `Pi` directory
- run `make`
- run `./Audio_Savage`
- go to `localhost:8000/index.html` 
  - if ssh into a machine go to `<SSH_IP_ADDRESS>:8000/index.html`
- Press any keyboard key on webpage and it print outs the keycode on the Pi terminal
- The current temperature will be display
  - To ramp up the temperature, run this bash script in a new terminal window to add some heat (just loops through 100k numbers)
  ```
  i=0; while [ $i -le 100000 ]; do if [ $(( ($i * 2 * 2 / 4) % 10000)) -eq 0 ]; then echo $i; fi; i=`expr $i + 1`; done
  ```

## Not working?
- Try refreshing the page
- Try running `make clean` then `make`
