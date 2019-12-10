module GenerateurPWM(SW,GPIO,CLOCK_50,KEY,DRAM_DQ,DRAM_DQM,DRAM_CKE,DRAM_CLK,DRAM_WE_N,
DRAM_CAS_N,DRAM_RAS_N,DRAM_CS_N,DRAM_BA,DRAM_ADDR,LCD_RS,LCD_RW,LCD_EN,LCD_DATA);

inout[31:0] DRAM_DQ;
output[3:0] DRAM_DQM;
output DRAM_CKE,DRAM_CLK,DRAM_WE_N,DRAM_CAS_N,DRAM_RAS_N,DRAM_CS_N;
output[1:0] DRAM_BA;
output[12:0] DRAM_ADDR;

input CLOCK_50;
input[3:0] KEY;
input[1:0] SW;

output[35:0] GPIO;

//LCD
inout[7:0] LCD_DATA;
output LCD_RS,LCD_RW,LCD_EN;

// Lien entre CPU et la PWM
wire PWMCLK;
wire[23:0] CLKMultiPWM;
wire[23:0] OE_GPIO;
wire[23:0] LATCH_GPIO;
wire[9:0] TON_GPIO;


	 
    Base_SDRAM_HAL_DE2_115_hard u0 (
        .clk_clk                 (CLOCK_50),                 //               clk.clk
        .reset_reset_n           (KEY[0]),           //             reset.reset_n
        .sdram_ctrler_wire_addr  (DRAM_ADDR[12:0]),  // sdram_ctrler_wire.addr
        .sdram_ctrler_wire_ba    (DRAM_BA[1:0]),    //                  .ba
        .sdram_ctrler_wire_cas_n (DRAM_CAS_N), //                  .cas_n
        .sdram_ctrler_wire_cke   (DRAM_CKE),   //                  .cke
        .sdram_ctrler_wire_cs_n  (DRAM_CS_N),  //                  .cs_n
        .sdram_ctrler_wire_dq    (DRAM_DQ[31:0]),    //                  .dq
        .sdram_ctrler_wire_dqm   (DRAM_DQM[3:0]),   //                  .dqm
        .sdram_ctrler_wire_ras_n (DRAM_RAS_N), //                  .ras_n
        .sdram_ctrler_wire_we_n  (DRAM_WE_N),  //                  .we_n
        .sdramclk_clk            (DRAM_CLK),            //          sdramclk.clk
        .pwmclk_clk              (PWMCLK),              //            pwmclk.clk
        .oegpio_export           (OE_GPIO[23:0]),            //            oegpio.export
        .latchgpio_export        (LATCH_GPIO[23:0]),        //         latchgpio.export
        .tongpio_export          (TON_GPIO[9:0]),          //           tongpio.export
		  .btngpio_export          (KEY[3:1]),          //           btngpio.export
		  .switchgpio_export       (SW[1:0]),        //        switchgpio.export
        .lcd_16207_RS            (LCD_RS),            //         lcd_16207.RS
        .lcd_16207_RW            (LCD_RW),            //                  .RW
        .lcd_16207_data          (LCD_DATA[7:0]),          //                  .data
        .lcd_16207_E             (LCD_EN)              //                  .E
    
        
    );	 

assign GPIO[35] = PWMCLK;//Renvoie de la clock pwm sur la broche 35
assign CLKMultiPWM[23:0] = {24{PWMCLK}}; //duplication du signal PWMCLK sur plusieurs fils
	 
modMultiPWM #(24,10) multiPWM( CLKMultiPWM[23:0], GPIO[23:0], LATCH_GPIO[23:0], TON_GPIO[9:0], OE_GPIO[23:0]);

endmodule
