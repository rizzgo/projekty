library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

-- rozhrani Vigenerovy sifry
entity vigenere is
   port(
         CLK : in std_logic;
         RST : in std_logic;
         DATA : in std_logic_vector(7 downto 0);
         KEY : in std_logic_vector(7 downto 0);

         CODE : out std_logic_vector(7 downto 0)
    );
end vigenere;

-- V souboru fpga/sim/tb.vhd naleznete testbench, do ktereho si doplnte
-- znaky vaseho loginu (velkymi pismeny) a znaky klice dle vaseho prijmeni.

architecture behavioral of vigenere is

    -- Sem doplnte definice vnitrnich signalu, prip. typu, pro vase reseni,
    -- jejich nazvy doplnte tez pod nadpis Vigenere Inner Signals v souboru
    -- fpga/sim/isim.tcl. Nezasahujte do souboru, ktere nejsou explicitne
    -- v zadani urceny k modifikaci.
	 
	 signal safe_data: std_logic_vector(7 downto 0) := "00000000";
	 signal safe_key: std_logic_vector(7 downto 0) := "00000000";
	 
	 signal difference: std_logic_vector(7 downto 0) := "00000000";
	 
	 signal higher_ascii: std_logic_vector(7 downto 0) := "00000000";
	 signal lower_ascii: std_logic_vector(7 downto 0) := "00000000";

	 constant state_r: std_logic_vector(1 downto 0) := "00";
	 constant state_0: std_logic_vector(1 downto 0) := "01";
	 constant state_1: std_logic_vector(1 downto 0) := "10";
	 
	 signal p_state: std_logic_vector(1 downto 0) := "00";
	 signal next_state: std_logic_vector(1 downto 0) := "00";
	 signal output: std_logic_vector(1 downto 0) := "00";
	 
begin

    -- Sem doplnte popis obvodu. Doporuceni: pouzivejte zakladni obvodove prvky
    -- (multiplexory, registry, dekodery,...), jejich funkce popisujte pomoci
    -- procesu VHDL a propojeni techto prvku, tj. komunikaci mezi procesy,
    -- realizujte pomoci vnitrnich signalu deklarovanych vyse.

    -- DODRZUJTE ZASADY PSANI SYNTETIZOVATELNEHO VHDL KODU OBVODOVYCH PRVKU,
    -- JEZ JSOU PROBIRANY ZEJMENA NA UVODNICH CVICENI INP A SHRNUTY NA WEBU:
    -- http://merlin.fit.vutbr.cz/FITkit/docs/navody/synth_templates.html.
	 
	 
	 --Kontrola, ci su DATA a KEY definovane (warning pri simulacii)
	 process(DATA, KEY)
	 begin
		if (DATA(0) = '0') or (DATA(0) = '1') then
			safe_data <= DATA;
		end if;
		if (KEY(0) = '0') or (KEY(0) = '1') then
			safe_key <= KEY;
		end if;
	 end process;
	
	 -- Vypocet velkosti posunu
	 process(safe_key)
	 begin
		difference <= safe_key - "01000000"; -- ascii kod znaku minus 64 je poradie pismena v abecede
	 end process;
	 
	 -- Posun s +
	 process(difference)
	 begin
		if (safe_data + difference <= "01011010") then -- ak nie je vysledok vyssi ako 'Z'
			higher_ascii <= safe_data + difference;
		else
			higher_ascii <= safe_data + difference - "00011010"; -- ak je vyssi ako 'Z', odpocita sa 26
		end if;
	 end process;
	 
	 -- Posun s -
	 process(difference)
	 begin
		if (safe_data - difference >= "01000001") then -- ak nie je vysledok nizsi ako 'A'
			lower_ascii <= safe_data - difference;
		else
			lower_ascii <= safe_data - difference + "00011010"; -- ak je nizsi ako 'A', pripocita sa 26
		end if;
	 end process;
	 
	 -- Zmena stavu Mealyho automatu pri nastupnej hrane
	 process(RST, CLK)
	 begin
		if (RST = '1') then
			p_state <= state_r;
		elsif (CLK'event) and (CLK = '1') then
			p_state <= next_state;
		end if;
	 end process;
	 
	 -- Nasledujuci stav a vystup Mealyho automatu
	 process(p_state, safe_data, RST)
	 begin
		case p_state is
			when state_r =>	
				if (safe_data >= "01000001") and (safe_data <= "01011010") and (RST = '0') then -- 'A' <= DATA <= 'Z'
					next_state <= state_0; -- po zruseni resetu mimo nastupnej hrany je dalsi posun s -
					output <= state_1; -- a na vystup ide posun s +
				else
					next_state <= state_r;
					output <= state_r;
				end if;
			when state_0 => 
				if (safe_data >= "01000001") and (safe_data <= "01011010") and (RST = '0') then
					next_state <= state_1; -- pri dalsom takte bude posun s +
					output <= state_0;
				else
					next_state <= state_r;
					output <= state_r;
				end if;
			when state_1 =>
				if (safe_data >= "01000001") and (safe_data <= "01011010") and (RST = '0') then
					next_state <= state_0; -- pri dalsom takte bude posun s -
					output <= state_1;
				else
					next_state <= state_r;
					output <= state_r;
				end if;
			when others => null;	
		end case;
	 end process;
	 
	 -- Vyber vystupu podla stavu automatu
	 with output select
		CODE <= higher_ascii when state_1, -- vysledok je posun s +
				  lower_ascii when state_0, -- vysledok je posun s -
				  "00100011" when others; -- ak nie -> vystup je '#'
	 
	 	
end behavioral;
