#pragma once

namespace SyntheticSkinWeapons
{
	struct Entry
	{
		int defIndex;
		const char* name;
	};

	inline Entry kList[] = {
		{ 7 , "AK-47" } , { 8 , "AUG" } , { 9 , "AWP" } , { 1 , "Desert Eagle" } , { 2 , "Dual Berettas" } ,
		{ 10 , "FAMAS" } , { 3 , "Five-SeveN" } , { 13 , "Galil AR" } , { 11 , "G3SG1" } , { 4 , "Glock-18" } ,
		{ 14 , "M249" } , { 16 , "M4A4" } , { 60 , "M4A1-S" } , { 17 , "MAC-10" } , { 27 , "MAG-7" } ,
		{ 33 , "MP7" } , { 34 , "MP9" } , { 23 , "MP5-SD" } , { 28 , "Negev" } , { 35 , "Nova" } ,
		{ 19 , "P90" } , { 32 , "P2000" } , { 36 , "P250" } , { 26 , "PP-Bizon" } , { 64 , "R8 Revolver" } ,
		{ 29 , "Sawed-Off" } , { 38 , "SCAR-20" } , { 39 , "SG 553" } , { 40 , "SSG 08" } , { 30 , "Tec-9" } ,
		{ 24 , "UMP-45" } , { 61 , "USP-S" } , { 25 , "XM1014" } , { 63 , "CZ75-Auto" } ,
		{ 500 , "Bayonet" } , { 505 , "Flip Knife" } , { 506 , "Gut Knife" } , { 507 , "Karambit" } ,
		{ 508 , "M9 Bayonet" } , { 509 , "Huntsman Knife" } , { 512 , "Falchion Knife" } , { 514 , "Bowie Knife" } ,
		{ 515 , "Butterfly Knife" } , { 516 , "Shadow Daggers" } , { 517 , "Paracord Knife" } , { 519 , "Ursus Knife" } ,
		{ 520 , "Navaja Knife" } , { 521 , "Nomad Knife" } , { 522 , "Stiletto Knife" } , { 523 , "Talon Knife" } ,
		{ 525 , "Skeleton Knife" } , { 526 , "Kukri Knife" } ,
	};

	inline constexpr int kCount = sizeof( kList ) / sizeof( kList[0] );
}
