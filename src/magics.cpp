#include "magics.h"

#include <random>
#include "bitwise_ops.h"
#include <iostream>
#include <board.h>

// Note: https://www.chessprogramming.org/Looking_for_Magics
// Note: https://www.chessprogramming.org/Magic_Bitboards
namespace
{
	constexpr int bsize = 5248;
	constexpr int rsize = 102400;
	bitboard attack_table_bishop[bsize];// This is just the sum of all BBits entries (sum(1<<entry0,1<<entry1,...)) 
	bitboard attack_table_rook[rsize];  // Same

	int RBits[64] = {
	  12, 11, 11, 11, 11, 11, 11, 12,
	  11, 10, 10, 10, 10, 10, 10, 11,
	  11, 10, 10, 10, 10, 10, 10, 11,
	  11, 10, 10, 10, 10, 10, 10, 11,
	  11, 10, 10, 10, 10, 10, 10, 11,
	  11, 10, 10, 10, 10, 10, 10, 11,
	  11, 10, 10, 10, 10, 10, 10, 11,
	  12, 11, 11, 11, 11, 11, 11, 12
	};

	int BBits[64] = {
	  6, 5, 5, 5, 5, 5, 5, 6,
	  5, 5, 5, 5, 5, 5, 5, 5,
	  5, 5, 7, 7, 7, 7, 5, 5,
	  5, 5, 7, 9, 9, 7, 5, 5,
	  5, 5, 7, 9, 9, 7, 5, 5,
	  5, 5, 7, 7, 7, 7, 5, 5,
	  5, 5, 5, 5, 5, 5, 5, 5,
	  6, 5, 5, 5, 5, 5, 5, 6
	};


	u64 magics_b[64]{
	38299357319020672ULL,
	4695029515217379592ULL,
	2252366757892166ULL,
	2306973446754017330ULL,
	649380644048076800ULL,
	288654925626376202ULL,
	4612830696200994976ULL,
	378426617942392864ULL,
	9223970205578297408ULL,
	8866976841984ULL,
	648676682491772930ULL,
	2099875937146423296ULL,
	9223376452634820928ULL,
	576466267578597920ULL,
	378426617942392864ULL,
	2252366757892166ULL,
	4503892222542096ULL,
	9224647608118052354ULL,
	1301549092702912704ULL,
	1125934287572992ULL,
	3459894813948649520ULL,
	37226173788364864ULL,
	18298083391053824ULL,
	1729946862112474112ULL,
	2322186287450372ULL,
	54329068686971924ULL,
	4613938917887049761ULL,
	1298162868608368642ULL,
	4684026187055382528ULL,
	1441997405301145730ULL,
	1195746417394780168ULL,
	4086431886869120ULL,
	2342452502995142661ULL,
	4611870771060674625ULL,
	4616229647360721028ULL,
	580966561700380800ULL,
	1127008008667268ULL,
	10092578043521450240ULL,
	10092578043521450240ULL,
	3477907442912854148ULL,
	290280733552682ULL,
	7494206418229141504ULL,
	18103529952808960ULL,
	11565384864052037644ULL,
	590575485059329ULL,
	10739400694376169984ULL,
	1157442973462435968ULL,
	4612257769389722720ULL,
	4612830696200994976ULL,
	4549814213935236ULL,
	291525234270216ULL,
	289373920363544576ULL,
	9229019200012550144ULL,
	9228227926905389072ULL,
	18375115700453376ULL,
	4695029515217379592ULL,
	378426617942392864ULL,
	2252366757892166ULL,
	193654853510369284ULL,
	1477185213280061440ULL,
	140742068666880ULL,
	288230394942475522ULL,
	9223970205578297408ULL,
	38299357319020672ULL,
	};

	u64 magics_r[64]{
	5800637591640358912ULL,
	18031991769276416ULL,
	1008815251145101824ULL,
	756622330255385728ULL,
	72062017988526096ULL,
	9259405231953805696ULL,
	144128416588365825ULL,
	36034294587605120ULL,
	2882444509747085412ULL,
	72198606406288512ULL,
	6917669833924419584ULL,
	288512023480897536ULL,
	90213279858690048ULL,
	18155153182032000ULL,
	579276086538534980ULL,
	9295992590002324483ULL,
	18014948267426888ULL,
	74811596056043522ULL,
	2305984296726978560ULL,
	9246174808639574016ULL,
	90213279858690048ULL,
	2450099484567274497ULL,
	2594077783428829192ULL,
	9223374235886567692ULL,
	288300747043382660ULL,
	162516617900589129ULL,
	1153486730895065152ULL,
	2260598062583808ULL,
	6918655479451746372ULL,
	72064193263567872ULL,
	110426203345126790ULL,
	9223372320322912516ULL,
	144531078391005312ULL,
	580964489373943108ULL,
	6917669833924419584ULL,
	10425850729733300480ULL,
	4629709217341640716ULL,
	563001526603784ULL,
	72145559330232326ULL,
	722283005229400132ULL,
	9077570691694592ULL,
	6917616989108322308ULL,
	301742411992891440ULL,
	5207568612862263336ULL,
	8942692466704ULL,
	4508273089184016ULL,
	579276086538534980ULL,
	162292315455619073ULL,
	864831870309771520ULL,
	72198606406288512ULL,
	90288669371400448ULL,
	17594350829696ULL,
	2954503193630871554ULL,
	18155153182032000ULL,
	378313398444688384ULL,
	144687604422427136ULL,
	180284725271744769ULL,
	151809930838050ULL,
	578854191549128770ULL,
	38302729067567361ULL,
	17294385537176576002ULL,
	14447829096761655873ULL,
	576619116891342980ULL,
	1009407203941286210ULL,
	};

	struct magic_info
	{
		u64 magic;
		int shift;
		bitboard* attack_table;
		bitboard mask;
	};

	magic_info magics_info_b[64];
	magic_info magics_info_r[64];


	/* Maps the i'th index to a line occupancy bitboard
	 For example in a 3x3 field:
	 lines_mask = 110
				  001
				  001 -> Here the rook stands on the upper right square.
	idx = 0 would now map to 000
							 000
							 000
	idx = 1 = 000 would map to 000
			  000		       000
			  001		       001
	idx = 2 = 000 would map to 000
			  000			   001
			  010			   000
	idx = 3 = 000 would map to 000
			  000			   001
			  011			   001
	etc.

	1. How it does this is: start with an empty board.
	2. Then pop the first set bit from
	from lines_mask -> remember the position
	3. If the first bit is set in the index, then we put the
	popped bit at the saved position on the empty board.
	4. Do this for all other bits, until we reach idx = n.
	In this way we correspond a set bit in the index to a set bit in the result.
	This creates a unique line occupcany bitboard.
	*/
	bitboard map_idx_to_line_occ(int idx, int n, bitboard lines_mask)
	{
		bitboard line_occ = 0;
		for (int i = 0; i < n; i++)
		{
			// Get the i'th set bit
			int j = ops::num_trailing_zeros(lines_mask);
			// Test if the i'th bit is set in index
			if (ops::is_idx_set(idx, i))
				// If yes, we put the popped bit back onto the line_occ 
				line_occ |= ops::set_nth_bit(j);
			ops::pop_lsb(lines_mask);
		}
		return line_occ;
	}

	// Generates a key from the line occupancy
	int hash(u64 magic, int shift, bitboard line_occ)
	{
		return (int)((line_occ * magic) >> (64 - shift));
	}

	void find_magic(int idx, int shift, bool bishop, u64* magics)
	{
		std::mt19937_64 gen(89237509826);
		std::uniform_int_distribution<u64> distr(
			std::numeric_limits<u64>::min(),
			std::numeric_limits<u64>::max()
		);

		auto rdn = [&]() {return distr(gen) & distr(gen) & distr(gen); };
		bitboard line_occs[4096]; // Maxium number of possible line occupancies excluding the outer squares
		bitboard attack[4096];


		// Generate the mask of the lines i.e 
		/*								  110
		*								  001
										  001 for a rook on the upper right square*/

										  // Always exclude the bit we are standing on.
		bitboard slider = ops::set_nth_bit(idx);
		bitboard file_mask_no_edges = ops::get_file_mask_excluding_edges(idx);
		bitboard file_mask = ops::get_file_mask(idx);
		bitboard rank_mask_no_edges = ops::get_rank_mask_excluding_edges(idx);
		bitboard diag_mask = ops::get_diag_mask(idx);
		bitboard anti_diag_mask = ops::get_antidiag_mask(idx);

		bitboard line_mask = bishop ? (diag_mask | anti_diag_mask) & ops::inner_board_mask()
			: file_mask_no_edges | rank_mask_no_edges;

		line_mask &= ~slider;
		/*if (!bishop)
		{
			std::cout << board::get_bitboard(line_mask) << "\n";
			return;
		}*/
		int num_bits = ops::count_number_of_set_bits(line_mask);
		// Iterate over all line occupancy configurations generated from the line mask
		for (int i = 0; i < (1 << num_bits); i++)
		{
			bitboard line_occ = map_idx_to_line_occ(i, num_bits, line_mask);
			line_occs[i] = line_occ;
			attack[i] = bishop ? ops::hyperbola_quintessence(line_occ, diag_mask, slider) | ops::hyperbola_quintessence(line_occ, anti_diag_mask, slider)
				: ops::hyperbola_quintessence(line_occ, file_mask, slider) | ops::hyperbola_quintessence_for_ranks(line_occ, slider);
		}

		u64 magic;
		while (true)
		{
			magic = rdn();
			u64 key_to_attack[4096] = { 0 };
			// Loop over all the possible configuration of the occupancy of the lines
			bool fail = false;
			for (int i = 0; i < (1 << num_bits); i++)
			{
				int key = hash(magic, shift, line_occs[i]);
				// Map the key to an attack.
				if (key_to_attack[key] == 0ULL) key_to_attack[key] = attack[i];
				// If we have already an attack saved in at the key position and it's different, we have a collision -> wrong magic
				else if (key_to_attack[key] != attack[i])
				{
					fail = true;
					break;
				}
			}
			if (!fail)
				break;
		}

		magics[idx] = magic;

	}
}

void init_attack_table()
{
	int offset_bishop = 0;
	int offset_rook = 0;
	for (int idx = 0; idx < 64; idx++)
	{
		for (int b = 0; b < 2; b++)
		{
			bool bishop = static_cast<bool>(b);
			// Always exclude the bit we are standing on.
			bitboard slider = ops::set_nth_bit(idx);
			bitboard file_mask_no_edges = ops::get_file_mask_excluding_edges(idx);
			bitboard file_mask = ops::get_file_mask(idx);
			bitboard rank_mask_no_edges = ops::get_rank_mask_excluding_edges(idx);
			bitboard diag_mask = ops::get_diag_mask(idx);
			bitboard anti_diag_mask = ops::get_antidiag_mask(idx);

			bitboard line_mask = bishop ? (diag_mask | anti_diag_mask) & ops::inner_board_mask()
				: file_mask_no_edges | rank_mask_no_edges;

			line_mask &= ~slider;
			// num_bits should correspond with either RBits[idx] or BBits[idx].
			int num_bits = ops::count_number_of_set_bits(line_mask);
			// Iterate over all line occupancy configurations generated from the line mask
			for (int i = 0; i < (1 << num_bits); i++)
			{
				bitboard line_occ = map_idx_to_line_occ(i, num_bits, line_mask);
				if (bishop)
				{
					u64 key = hash(magics_b[idx], BBits[idx],line_occ);
					attack_table_bishop[offset_bishop+key] = ops::hyperbola_quintessence(line_occ, diag_mask, slider) | ops::hyperbola_quintessence(line_occ, anti_diag_mask, slider);
				}
				else
				{
					u64 key = hash(magics_r[idx], RBits[idx], line_occ);
					attack_table_rook[offset_rook+key] = ops::hyperbola_quintessence(line_occ, file_mask, slider) | ops::hyperbola_quintessence_for_ranks(line_occ, slider);
				}
			}
			if(bishop) offset_bishop += 1<<BBits[idx];
			else offset_rook += 1<<RBits[idx];
		}

	}
}




void magic::init_magics()
{
	
	init_attack_table();

	int offset_r = 0;
	int offset_b = 0;
	for (int i = 0; i < 64; i++)
	{
		bitboard slider = ops::set_nth_bit(i);
		bitboard file_mask_no_edges = ops::get_file_mask_excluding_edges(i);
		bitboard file_mask = ops::get_file_mask(i);
		bitboard rank_mask_no_edges = ops::get_rank_mask_excluding_edges(i);
		bitboard diag_mask = ops::get_diag_mask(i);
		bitboard anti_diag_mask = ops::get_antidiag_mask(i);

		bitboard line_mask_bishop = (diag_mask | anti_diag_mask) & ops::inner_board_mask(); // Diag mask and Antidiag mask already exclude the slider.
		bitboard line_mask_rook = (file_mask_no_edges | rank_mask_no_edges) &~slider;
		magics_info_r[i] = { magics_r[i],64-RBits[i],attack_table_rook+offset_r,line_mask_rook };
		magics_info_b[i] = { magics_b[i],64-BBits[i],attack_table_bishop+offset_b,line_mask_bishop };
		offset_r += 1 << RBits[i];
		offset_b += 1 << BBits[i];
	}
}

bitboard magic::bishop_attacks(bitboard occ, int idx)
{
	auto& info = magics_info_b[idx];
	//std::cout << (info.mask & occ) << std::endl;
	//std::cout << board::get_bitboard(info.mask & occ) << std::endl;
	u64 key = (info.magic * (info.mask & occ)) >> info.shift;
	return info.attack_table[key];
}
bitboard magic::rook_attacks(bitboard occ, int idx)
{
	auto& info = magics_info_r[idx];
	u64 key = (info.magic * (info.mask & occ)) >> info.shift;
	return info.attack_table[key];
}


void magic::print_magics()
{
	u64 magics_b[64] = { 0 };
	u64 magics_r[64] = { 0 };
	std::cout << "u64 magics_b[64]{" << std::endl;
	for (int idx = 0; idx < 64; idx++)
	{
		find_magic(idx, BBits[idx], true, magics_b);
	}
	for (int i = 0; i < 64; i++)
	{
		std::cout << magics_b[i] << "ULL," << std::endl;
	}

	std::cout << "};" << std::endl << std::endl;

	std::cout << "u64 magics_r[64]{" << std::endl;
	for (int idx = 0; idx < 64; idx++)
	{
		find_magic(idx, RBits[idx], false, magics_r);
	}
	for (int i = 0; i < 64; i++)
	{
		std::cout << magics_r[i] << "ULL," << std::endl;
	}

	std::cout << "};" << std::endl << std::endl;
}