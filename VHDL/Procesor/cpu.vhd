-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2020 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xseipe00
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- ram[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_WE    : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti 
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
	
 -- pc
	signal pc_reg : std_logic_vector(11 downto 0);
	signal pc_inc : std_logic;
	signal pc_dec : std_logic;
	signal pc_ld  : std_logic;
	
 -- ras
	type buff is array (15 downto 0) of std_logic_vector(11 downto 0);
	signal ras_buff: buff;
	signal ras_npos : std_logic_vector(4 downto 0);
	signal ras_push : std_logic;
	signal ras_pop  : std_logic;
	
 -- ptr
	signal ptr_reg : std_logic_vector(9 downto 0);
	signal ptr_inc : std_logic;
	signal ptr_dec : std_logic;
	
 -- mx
	signal sel : std_logic_vector(1 downto 0);
	
 -- cnt
	signal   cnt_reg : std_logic_vector(7 downto 0);
	signal   cnt_inc : std_logic;
	signal   cnt_dec : std_logic;
	
 -- fsm
	type fsm_state is (sidle, sfetch, sdecode, sptr_inc, sptr_dec,
							 sdata_inc, sdata_dec, sbracket_l, sbracket_r,
							 senable_bracket_l, swhile_bracket_l,
							 sput, sget, snull, sothers);
	signal pstate : fsm_state;
	signal nstate : fsm_state;

begin

	-- pc
	pc: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pc_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if (pc_ld = '1') then
				pc_reg <= ras_buff(conv_integer(ras_npos - 1));
			elsif (pc_inc = '1') then
				pc_reg <= pc_reg + 1;
			elsif (pc_dec = '1') then
				pc_reg <= pc_reg - 1;
			end if;
		end if;
	end process;
	
	CODE_ADDR <= pc_reg;
	
	-- ras
	ras: process (RESET, CLK)
	begin
		if (RESET = '1') then
			ras_buff <= (others => "000000000000");
			ras_npos <= (others => '0'); -- ras next position (v buffri)
		elsif (CLK'event) and (CLK = '1') then
			if (ras_push = '1') then
				ras_buff(conv_integer(ras_npos)) <= pc_reg + 1;
				ras_npos <= ras_npos + 1;
			elsif (ras_pop = '1') then
				ras_buff(conv_integer(ras_npos - 1)) <= "000000000000";
				ras_npos <= ras_npos - 1;
			end if;
		end if;
	end process;
	
	-- cnt
	cnt: process (RESET, CLK)
	begin
		if (RESET = '1') then
			cnt_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if (cnt_inc = '1') then
				cnt_reg <= cnt_reg + '1';
			elsif (cnt_dec = '1') then
				cnt_reg <= cnt_reg - '1';
			end if;
		end if;
	end process;
	
	-- ptr
	ptr: process (RESET, CLK)
	begin
		if (RESET = '1') then
			ptr_reg <= (others => '0');
		elsif (CLK'event) and (CLK = '1') then
			if (ptr_inc = '1') then
				ptr_reg <= ptr_reg + 1;
			elsif (ptr_dec = '1') then
				ptr_reg <= ptr_reg - 1;
			end if;
		end if;
	end process;
	
	DATA_ADDR <= ptr_reg;
	
	-- mx
	with sel select
		DATA_WDATA <=  IN_DATA when "00",
							DATA_RDATA - 1 when "01",
							DATA_RDATA + 1 when "10",
							DATA_RDATA when others;
								
								
	-- fsm --
	fsm_pstate: process (RESET, CLK)
	begin
		if (RESET = '1') then
			pstate <= sidle;
		elsif (CLK'event) and (CLK = '1') then
			if (EN = '1') then
				pstate <= nstate;
			end if;
		end if;
	end process;
	
	fsm_nstate: process (pstate, CODE_DATA, DATA_RDATA, cnt_reg, OUT_BUSY, IN_VLD)
	begin
		CODE_EN <= '0';
		IN_REQ <= '0';
		OUT_WE <= '0';
		pc_inc <= '0';
		pc_dec <= '0';
		pc_ld <= '0';
		ras_push <= '0';
		ras_pop <= '0';
		cnt_inc <= '0';
		cnt_dec <= '0';
		ptr_inc <= '0';
		ptr_dec <= '0';
		sel <= "00";
		DATA_WE <= '0';
		DATA_EN <= '0';
								
		case pstate is
			when sidle =>
				nstate <= sfetch;
			
			when sfetch =>
				nstate <= sdecode;
				CODE_EN <= '1';
			
			when sdecode =>
				case CODE_DATA is
					when x"3E" =>
						nstate <= sptr_inc;
					when x"3C" =>
						nstate <= sptr_dec;
					when x"2B" =>
						nstate <= sdata_inc;
						DATA_EN <= '1';
					when x"2D" =>
						nstate <= sdata_dec;
						DATA_EN <= '1';
					when x"5B" =>
						nstate <= sbracket_l;
						DATA_EN <= '1';
					when x"5D" =>
						nstate <= sbracket_r;
						DATA_EN <= '1';
					when x"2E" =>
						nstate <= sput;
						DATA_EN <= '1';
					when x"2C" =>
						nstate <= sget;
					when x"00" =>
						nstate <= snull;
					when others =>
						nstate <= sothers;
				end case;
		
			when sptr_inc =>
				nstate <= sfetch;
				pc_inc <= '1';
				
				ptr_inc <= '1';
			
			when sptr_dec =>
				nstate <= sfetch;
				pc_inc <= '1';
				
				ptr_dec <= '1';
			
			when sdata_inc =>
				nstate <= sfetch;
				pc_inc <= '1';
				
				sel <= "10";
				DATA_WE <= '1';
				DATA_EN <= '1';
			
			when sdata_dec =>
				nstate <= sfetch;
				pc_inc <= '1';
				
				sel <= "01";
				DATA_WE <= '1';
				DATA_EN <= '1';
				
			when sbracket_l =>
				pc_inc <= '1';
				if (DATA_RDATA = x"00") then
					cnt_inc <= '1';
					nstate <= senable_bracket_l; -- vstup do while
				else
					ras_push <= '1';
					nstate <= sfetch;
				end if;
				
			when senable_bracket_l =>  --- while cyklus
					CODE_EN <= '1';
					nstate <= swhile_bracket_l;
				
			when swhile_bracket_l =>
				if (cnt_reg /= x"00") then
					if (CODE_DATA = x"5B") then
						cnt_inc <= '1';
					elsif (CODE_DATA = x"5D") then
						cnt_dec <= '1';
					end if;
					pc_inc <= '1';
					nstate <= senable_bracket_l;
				else
					nstate <= sfetch;
				end if;						--- while cyklus end
			
			when sbracket_r =>
				nstate <= sfetch;
				
				if (DATA_RDATA = x"00") then
					pc_inc <= '1';
					ras_pop <= '1';
				else
					pc_ld <= '1';
				end if;
				
			when sput =>
			
				if (OUT_BUSY = '0') then
					pc_inc <= '1';
					OUT_WE <= '1';
					nstate <= sfetch;
				else
					nstate <= sput;
				end if;
				
			when sget =>
				IN_REQ <= '1';
				
				if (IN_VLD = '1') then
					pc_inc <= '1';
					DATA_WE <= '1';
					DATA_EN <= '1';
					nstate <= sfetch;
				else
					nstate <= sget;
				end if;
				
			when snull =>
				nstate <= snull;
				
			when others =>
				nstate <= sfetch;
				pc_inc <= '1';
				
		end case;
	end process;
	
	OUT_DATA <= DATA_RDATA when (OUT_BUSY = '0') and (pstate = sput) else
					(others => '0') when (RESET = '1');


end behavioral;
 
