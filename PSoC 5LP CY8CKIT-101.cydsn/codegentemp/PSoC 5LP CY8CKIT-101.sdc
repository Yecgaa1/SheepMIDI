# THIS FILE IS AUTOMATICALLY GENERATED
# Project: G:\SheepMIDI\PSoC 5LP CY8CKIT-101.cydsn\PSoC 5LP CY8CKIT-101.cyprj
# Date: Wed, 08 Jan 2025 07:35:28 GMT
#set_units -time ns
create_clock -name {WaveDAC8_DacClk(routed)} -period 124999.99999999997 -waveform {0 62500} [list [get_pins {ClockBlock/dclk_2}]]
create_clock -name {Clock_500Hz(routed)} -period 2000000 -waveform {0 1000000} [list [get_pins {ClockBlock/dclk_5}]]
create_clock -name {CyILO} -period 10000 -waveform {0 5000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 27.777777777777771 -waveform {0 13.8888888888889} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 27.777777777777771 -waveform {0 13.8888888888889} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {ADC_theACLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 21 41} [list [get_pins {ClockBlock/aclk_glb_0}]]
create_generated_clock -name {UART_2_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 39 79} -nominal_period 1083.333333333333 [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {UART_1_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 39 79} -nominal_period 1083.333333333333 [list [get_pins {ClockBlock/dclk_glb_1}]]
create_generated_clock -name {WaveDAC8_DacClk} -source [get_pins {ClockBlock/clk_sync}] -edges {1 4501 9001} -nominal_period 124999.99999999997 [list [get_pins {ClockBlock/dclk_glb_2}]]
create_generated_clock -name {Clk_Tmr_CH1} -source [get_pins {ClockBlock/clk_sync}] -edges {1 36001 72001} -nominal_period 999999.99999999977 [list [get_pins {ClockBlock/dclk_glb_3}]]
create_generated_clock -name {Clk_Tmr_Button} -source [get_pins {ClockBlock/clk_sync}] -edges {1 36001 72001} -nominal_period 999999.99999999977 [list [get_pins {ClockBlock/dclk_glb_4}]]
create_generated_clock -name {Clock_500Hz} -source [get_pins {ClockBlock/clk_sync}] -edges {1 72001 144001} [list [get_pins {ClockBlock/dclk_glb_5}]]


# Component constraints for G:\SheepMIDI\PSoC 5LP CY8CKIT-101.cydsn\TopDesign\TopDesign.cysch
# Project: G:\SheepMIDI\PSoC 5LP CY8CKIT-101.cydsn\PSoC 5LP CY8CKIT-101.cyprj
# Date: Wed, 08 Jan 2025 07:35:23 GMT
