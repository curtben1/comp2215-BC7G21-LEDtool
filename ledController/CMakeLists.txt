add_executable(ledController
        ledController.c
        )

# pull in common dependencies
target_link_libraries( ledController pico_stdlib hardware_gpio hardware_pwm hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(ledController)

# add url via pico_set_program_url
example_auto_set_url(ledController)
