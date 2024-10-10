/*	SCCS Id: @(#)mplayer.c	3.4	1997/02/04	*/
/*	Copyright (c) Izchak Miller, 1992.			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"


STATIC_DCL const char *NDECL(dev_name);
STATIC_DCL void FDECL(get_mplname, (struct monst *, char *, boolean));
STATIC_DCL void FDECL(get_mplname_culture, (struct monst *, char *));
STATIC_DCL void FDECL(mk_mplayer_armor, (struct monst *, int));

/* These are the names of those who
 * contributed to the development of NetHack 3.2/3.3/3.4.
 *
 * Keep in alphabetical order within teams.
 * Same first name is entered once within each team.
 */
static const char *developers[] = {
	/* devteam */
	"Dave", "Dean", "Eric", "Izchak", "Janet", "Jessie",
	"Ken", "Kevin", "Michael", "Mike", "Pat", "Paul", "Steve", "Timo",
	"Warwick",
	/* PC team */
	"Bill", "Eric", "Keizo", "Ken", "Kevin", "Michael", "Mike", "Paul",
	"Stephen", "Steve", "Timo", "Yitzhak",
	/* Amiga team */
	"Andy", "Gregg", "Janne", "Keni", "Mike", "Olaf", "Richard",
	/* Mac team */
	"Andy", "Chris", "Dean", "Jon", "Jonathan", "Kevin", "Wang",
	/* Atari team */
	"Eric", "Marvin", "Warwick",
	/* NT team */
	"Alex", "Dion", "Michael",
	/* OS/2 team */
	"Helge", "Ron", "Timo",
	/* VMS team */
	"Joshua", "Pat",
	""};


/* return a randomly chosen developer name */
STATIC_OVL const char *
dev_name()
{
	register int i, m = 0, n = SIZE(developers);
	register struct monst *mtmp;
	register boolean match;

	do {
	    match = FALSE;
	    i = rn2(n);
	    for (mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		if(!is_mplayer(mtmp->data)) continue;
		if(!M_HAS_NAME(mtmp)) continue;
		if(!strncmp(developers[i], MNAME(mtmp),
			               strlen(developers[i]))) {
		    match = TRUE;
		    break;
	        }
	    }
	    m++;
	} while (match && m < 100); /* m for insurance */

	if (match) return (const char *)0;
	return(developers[i]);
}

STATIC_OVL void
get_mplname(mtmp, nam, endgame)
register struct monst *mtmp;
char *nam;
boolean endgame;
{
	boolean fmlkind = is_female(mtmp->data);
	if(endgame){
		const char *devnam;

		devnam = dev_name();
		if (!devnam)
			Strcpy(nam, fmlkind ? "Eve" : "Adam");
		else if (fmlkind && !!strcmp(devnam, "Janet"))
			Strcpy(nam, rn2(2) ? "Maud" : "Eve");
		else Strcpy(nam, devnam);

		if (fmlkind || !strcmp(nam, "Janet"))
			mtmp->female = 1;
		else
			mtmp->female = 0;
		Strcat(nam, " the ");
		Strcat(nam, rank_of((int)mtmp->m_lev,
					monsndx(mtmp->data),
					(boolean)mtmp->female));
	}
	else {
		get_mplname_culture(mtmp, nam);
	}
}

STATIC_OVL void
get_mplname_culture(mtmp, nam)
register struct monst *mtmp;
char *nam;
{
	boolean fmlkind = mtmp->female;
	const char *namelist;
	switch(mtmp->mtyp){
	case PM_ARCHEOLOGIST:{
		//"Indiana Jones": US state followed by common name
		const char *states[] = {
			"Alabama", "Alaska", "Arizona", "Arkansas", 
			"California", "Colorado", "Connecticut", 
			"Delaware", "Florida", "Georgia", "Hawaii", 
			"Idaho", "Illinois", "Indiana", "Iowa", 
			"Kansas", "Kentucky", "Louisiana", 
			"Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota", "Mississippi", "Missouri", "Montana", 
			"Nebraska", "Nevada", "New Hampshire", "New Jersey", "New Mexico", "New York", 
			"North Carolina", "North Dakota", 
			"Ohio", "Oklahoma", "Oregon", "Pennsylvania", "Rhode Island", "South Carolina", "South Dakota", 
			"Tennessee", "Texas", "Utah", "Vermont", "Virginia", "Washington", "West Virginia", "Wisconsin", "Wyoming"
		};
		const char *masculine[] = {
			// "Smith", "Johnson", "Williams", "Brown", "Jones"
			"Smith", "John", "Will", "Brown", "Jones"
		};
		const char *feminine[] = {
			// "Susan", "Jane", "Mary", "Babbs", "Jessica"
			"Sue", "Jane", "Mary", "Babbs", "Jess"
		};
		sprintf(nam, "%s %s", ROLL_FROM(states), fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_ANACHRONONAUT:{
		//Character names from the Terminator franchise
		//Star Wars film names, avoiding Jedi names basically because Obi-Wan doesn't seem like it's mix and match well
		//The two named characters from the Night Land
		const char *masculine[] = {
			"John", "Marcus", "Kyle", "Peter", "Ed", "Vukovich", "Miles", "Enrique",
			
			"Anakin", "Luke", "Han", "Raymus", "Wedge", "Biggs", "Owen", 
			"Lando", "Bib", "Sio", "Poe", "Finn",
			
			"Aschoff"
		};
		const char *feminine[] = {
			"Sarah", "Grace", "Cameron", "Catherine", "Jessica", "Kate",
			
			"Beru", "Leia", "Mon", "Oola", "Padme", "Sabe", "Shmi",
			"Corde", "Dome", "Rey", "Rose",

			"Naani"
		};
		const char *family[] = {
			"Write", "Harper", "Henry", "Weaver", "Connor", "Reese",
			"Silberman","Traxler","Dyson","Salceda","Brewster",
			
			"Antilles", "Darklighter", "Lars", "Organa", "Solo",
			"Skywalker", "Calrissian", "Fortuna", "Bibble", 
			"Dameron"
		};
		sprintf(nam, "%s %s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine), ROLL_FROM(family));
	}break;
	case PM_BARBARIAN:{
		//Hat tip: Characters of the Hyborian Age, hyboria.xoth.net
		const char *masculine[] = {
			"Abdashtarth", "Afari", "Ageera", "Aja", "Ajaga", 
			"Ajonga", "Akhirom", "Akkutho", "Alafdhal", "Alcemides", 
			"Almuric", "Altaro", "Amalric", "Amalric", "Amalric", 
			"Amboola", "Amra", "Amurath, Shah", "Angharzeb", "Antar", 
			"Aram Baksh", "Aratus", "Arbanus", "Arideus", "Arpello", 
			"Arshak", "Arshak", "Artaban", "Arus", "Askia", "Astreas", 
			"Atalis", "Athicus", "Attelius", "Aztrias Petanius", 
			"Bajujh", "Balthus", "Bardiya", "Beloso", "Bhunda Chand", 
			"Bit-Yakin", "Bombaata", "Bragi", "Bragoras", "Brant Drago", 
			"Baal", "Baal-Pteor", "Chicmec", "Chiron", "Chunder Shan", 
			"Codrus", "Constantius", "Ctesphon", "Dathan", "Dayuki", 
			"Demetrio", "Demetrio", "Dexitheus", "Dion", "Dionus", 
			"Dirk Strom", "Emilius Scavonus", "Enaro", "Epemitreus", 
			"Epeus", "Escelan", "Farouz", "Fronto", "Galacus", "Galbro", 
			"Gault Hagar", "Gebellez", "Ghaznavi", "Gilzan", "Gleg", 
			"Gobir", "Gorm", "Gorulga", "Gotarza", "Gromel", "Gwarunga", 
			"Hadrathus", "Hakhamalu", "Hakon Strom", "Hattusas", 
			"Heimdul", "Horsa", "Hotep", "Imbalayo", "Ishbak", "Ivanos", 
			"Jamal", "Jehungir", "Jehungir Agha", "Jelal Khan", "Joka", 
			"Jungir Khan", "Kallian Publico", "Karanthes", "Karlus", 
			"Keluka", "Keraspa", "Kerim Shah", "Khafra", "Khannon", 
			"Khaza", "Khemsa", "Khosatral Khel", "Khosrun Khan", 
			"Khossus", "Khumbanigash", "Khurum", "Kobad Shah", "Kordofo", 
			"Krallides", "Kujala", "Kurush Khan", "Kutamun", "Mattenbaal", 
			"Mazdak", "Moranthes", "Munthassem Khan", "Murilo", "Nabonidus", 
			"Natohk", "Nestor", "N'Gora", "Nimed", "Niord", "Numa", "Numedides",
			"N'Yaga", "Olgerd Vladislav", "Olmec", "Orastes", "Ortho", "Ostono",
			"Otho Gorm", "Pallantides", "Pelias", "Petreus", "Posthumo", "Promero",
			"Prospero", "Publio", "Publius", "Rammon", "Rinaldo", "Rustum", "Saidu",
			"Sakumbe", "Sassan", "Sergius", "Servio", "Servius Galannus",
			"Shaf Karaz", "Shevatas", "Shubba", "Shukeli", "Shupras", "Skelos",
			"Soractus", "Strabonus", "Sumuabi", "Tachic", "Tananda", "Tarascus",
			"Taurus", "Taurus", "Techotl", "Teyanoga", "Teyaspa", "Than", "Thasperas",
			"Thespides", "Thespius", "Theteles", "Thoth-Amon", "Thothmekri",
			"Thugra Khotan", "Thutmekri", "Thutothmes", "Tiberias", "Tiberias",
			"Tiberio", "Tilutan", "Tito", "Tolkamec", "Topal", "Tothmekri",
			"Totrasmek", "Tranicos", "Trocero", "Tsotha-Lanti", "Tubal", "Tuthamon",
			"Tuthmes", "Ura", "Uriaz", "Valannus", "Valannus", "Valbroso", "Valenso",
			"Valerian", "Valerius", "Valerius", "Vathelos", "Vilerus", "Vinashko",
			"Virata", "Volmana", "Wulfhere", "Xaltotun", "Xatmec", "Yar Afzal",
			"Yara", "Yasunga", "Yezdigerd", "Yildiz", "Zahak", "Zal", "Zang",
			"Zapayo Da Kova", "Zaporavo", "Zarallo", "Zargheba", "Zingelito",
			"Zlanath", "Zogar Sag", "Zorathus", "Zyras"
		};
		const char *feminine[] = {
			"Akivasha", "Albiona", "Belesa", "Belit", "Catlaxoc",
			"Chabela", "Diana", "Gitara", "Hildico", "Ilga", "Ivga",
			"Jehnna", "Junia", "Kang Lou-Dze", "Khushia", "Kwarada", "Livia",
			"Marala", "Muriela", "Nafertari", "Nanaia", "Natala", "Nzinga",
			"Octavia", "Olivia", "Purasati", "Radegund", "Rann", "Rima",
			"Roxana", "Rufia", "Salome", "Sancha", "Shanya", "Taramis",
			"Thanara", "Tina", "Valeria", "Vammatar", "Vateesa", "Yasala",
			"Yasmela", "Yasmina", "Yateli", "Yelaya", "Zelata", "Zenobia",
			"Zeriti", "Zillah", "Zosara"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_HALF_DRAGON:{
		//Some dragons (or more likely, types of "dragons") from around the world.
		// h.t. wikipedia
		const char *dragons[] = {
			"Kirimu", "Bida", "Grootslang", "Monyohe", "Masingi",
			"Tarasque", "Fernyiges", "Tarantasio", "Wawel",
			"Illuyanka", "Lotan", "Zahhak", "Apalala",
			"Kaliya", "Bakunawa","Panlong","Tatsu",
			"Imoogi","Kihawahine","Gaasyendietha"
		};
		sprintf(nam, "%s", ROLL_FROM(dragons));
	}break;
	case PM_BARD:{
		//The muses, pythagorean philosophers, and bards from the British isles
		const char *masculine[] = {
			"Pythagoras","Hesiod","Homer","Philolaus","Archytas",
			"Amergin", "Domhnall", "Aneirin", "Taliesin"
		};
		const char *feminine[] = {
			"Calliope", "Euterpe", "Terpsichore","Clio","Thalia",
			"Melpomene","Polyhymnia","Erato","Urania","Aoide",
			"Melete","Mneme"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_CAVEMAN:
	case PM_CAVEWOMAN:{
		//Names associated with the Epic of Gilgamesh, wikipedia
		//Oldest known names (maybe)
		const char *masculine[] = {
			"Gilgamesh", "Enkidu", "Urshanabi", "Utanapishtim", "Shangashu", 
			"Akka", "Birhurturra","Lugalgabagal","Sin-leqi-unninni","Ur-Nungal",

			"Kushim", "Gal-Sal", "En-pap"
		};
		const char *feminine[] = {
			"Ninsun", "Shamhat", "Siduri", "Enmebaragesi", "Pestur",
			
			"Sukkalgir"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_CONVICT:{
		//Waterdeep (And Neverwinter, oops)
		const char *masculine[] = {
			"Aarin", "Desther", "Fenthick", "Kurth", "Baram",
			"Vengaul", "Floon", "Dalakhar", "Colstan","Marune",
			"Rhinnom","Ahmaergo","Nar'l","Noska","Ott"
			
		};
		const char *feminine[] = {
			"Aribeth", "Sharwyn", "Nathyrra", "Aurora","Avaereene",
			"Kerindra","Shindia","Arizza","Xibrindas","Nakoto"
		};
		const char *family[] = {
			"Gend", "de Tylmarande", "Indelayne", "Moss","Bloodsail",
			"Blagmaar", "Amcathra", "Cassalanter","Wand", "Rhuul",
			"Lynnrenno","Dannihyr","Darkeyes","Ur'gray","Steeltoes"
		};
		sprintf(nam, "%s %s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine), ROLL_FROM(family));
	}break;
	case PM_EXILE:{
		//Gnostics
		const char *masculine[] = {
			"Dositheos", "Simon", "Menander","Elkhasai",
			"Seth","Valentinus","Basilides","Thomas",
			"Marcion","Cerinthus",
		};
		const char *feminine[] = {
			"Mary","Florence","Marcellina","Flora",
			"Helena", "Philumena"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_HEALER:{
		//Greek healers, h/t wikipedia
		const char *masculine[] = {
			"Herophilus","Aristotle","Theophrastus","Erasistratus",
			"Galen","Dioscorides","Herodicus","Paean","Telesphorus"
		};
		const char *feminine[] = {
			"Agnodice","Eileithyia","Epione","Aceso","Aegle"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_INCANTIFIER:{
		//Planescape
		const char *masculine[] = {
			"Tivvum"
		};
		const char *feminine[] = {
			"Alluvius"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_KNIGHT:{
		/*Wikipedia's list of common knights of the round table, omitting Arther and Mordred.*/
		const char *masculine[] = {
			"Accolon", "Agloval", "Agravaine", "Bagdemagus", "Bedivere",
			"Bors", "Brunor", "Cador", "Calogrenant", "Caradoc",
			"Claudin", "Constantine", "Dagonet", "Daniel", "Dinadan",
			"Ector", "Elyan", "Erec", "Esclabor", "Feirefiz",
			"Gaheris", "Galahad", "Galehault", "Galeschin", "Gareth",
			"Gawain", "Gingalain", "Gornemant", "Griflet", "Hector",
			"Hoel", "Kay", "Lamorak", "Lancelot", "Lanval",
			"Leodegrance", "Lionel", "Lucan", "Maleagant", "Morholt",
			"Morien", "Palamedes", "Pelleas", "Pellinore", "Percival",
			"Safir", "Sagramore", "Segwarides", "Tor", "Tristan",
			"Urien", "Yvain"
		};
		const char *feminine[] = {
			"Anna", "Amite",
			"Blanchefleur", "Brangaine", "Caelia", "Creiddylad", 
			"Dindrane", "Elaine of Astolat", "Elaine of Benoic",
			"Elaine of Corbenic", "Elaine of Garlot",
			"Elaine of Listenoise", "Elaine the Peerless",
			"Enide", "Ettarre", "Evaine",
			"Geraintdagger", "Guinevak",
			"Heliabel", "Igraine", "Iseult", 
			"Laudine", "Lunete", "Lynette", "Lyonesse",
			"Morgause", "Morvydd",
			"Olwen", "Orgeluse",
			"Parcenet", "Ragnell",
			"Viviane"
		};
		if(fmlkind){
			sprintf(nam, "Dame %s", ROLL_FROM(feminine));
		}
		else {
			sprintf(nam, "Sir %s", ROLL_FROM(masculine));
		}
	}break;
	case PM_MONK:{
		//Character names from Journey to the West, "transcribed" via Google Translate.
		// Some include titles (or are titles)
		const char *masculine[] = {
			"Bajie", "Wujing", "Jinjie Shiba Gong", "Guzhi Gong",
			"Lingkongzi", "Fuyun Sou", "Dabai Gui"
		};
		const char *feminine[] = {
			"Cuilan", "Guowang", "De Yong Furen","Yutu Jing", "Su'e",
			
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_MADMAN:
	case PM_MADWOMAN:{
		//Characters from Lovecraft et al.
		const char *masculine[] = {
			"Abdul","George", "Goodenough", "Henry", "Zadok","Atal",
			"Barzai","Enoch","Randolph","Crom-Ya",
			"Joseph","Danforth","Edward","Pickman",
			"William","Walter","Gustaf","Klarkash-Ton",
			"Kuranes","John","Raymond","Luveh-Keraphf",
			"Obadiah","Nathaniel","T'yog","Ephraim",
			"Charles","Dexter","Herbert"
		};
		const char *feminine[] = {
			"Zee-liah", "Keziah","Abigail","Asenath","Lavinia",
			"Lilith",
			"Mary Emily", "Marie Anne", "Helena",
			"Kassandra",
		};
		const char *family[] = {
			"Akeley","Allen","Angell","Armitage","Atwood","Bowen",
			"Carter","Curwen","Derby","Dyer","Gilman","Johansen",
			"Legrasse","Marsh","Mason","Peaslee","Prinn","Waite",
			"Ward","West","Whateley"
		};
		sprintf(nam, "%s %s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine), ROLL_FROM(family));
	}break;
	case PM_NOBLEWOMAN:
	case PM_NOBLEMAN:{
		//Romanian nobility/kings and queens
		const char *masculine[] = {
			"Nicolae","Vladislav","Radu","Dan","Vlad","Mihail",
			"Alexandru","Mircea","Mihnea","Neagoe","Teodosie",
			"Moise","Patrascu","Petru"
		};
		const char *feminine[] = {
			"Margareta","Maria","Clara","Anna","Kalinikia","Anca",
			"Cneajna","Neacsa","Justina","Rada","Catherine","Smaranda",
			"Milica","Voica","Ruxandra","Zamfira","Stana","Chiajna","Jelena",
			"Neaga"
		};
		const char *family[] = {
			"Basarab","Bogdan-Musat","Caradja","Danesti","Draculesti",
			"Dragos","Mavrogheni","Mocioni","Soldan", "Ypsilantis"
		};
		if(fmlkind){
			sprintf(nam, "Lady %s %s", ROLL_FROM(feminine), ROLL_FROM(family));
		}
		else {
			sprintf(nam, "Lord %s %s", ROLL_FROM(masculine), ROLL_FROM(family));
		}
	}break;
	case PM_PRIEST:
	case PM_PRIESTESS:{
		//Medieval monks and nuns. h.t. wikipedia
		const char *masculine[] = {
			"Aelfwine", "Aelfwold", "Alan", "Aldhelm", "Aldred",
			"Bercthun", "Billfrith", "Botwine", "Byrhtferth",
			"Ceolwulf", "Eadberht", "Eadfirth", "Eata", "Henry",
			"Fulk", "Folcard", "Geoffrey", "Gervase", "Gregory",
			"Huna", "John", "Lantfred", "Lawrence", "Leofwynn",
			"Odulf", "Osbern", "Reginald", "Richard", "Roger",
			"Robert", "Symeon", "Tancred", "Torthred",
			"Uthred", "William", "Wynthryth", "Ymar"
		};
		const char *feminine[] = {
			"Agnes", "Angeline", "Anna", "Catherine", "Clare", "Juliana",
			"Chiara", "Emilia", "Giovanna", "Humility", "Margaret",
			"Michelina", "Giulia", "Elizabeth", "Stanislava", "Tova",
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_PIRATE:{
		//Pirate names
		const char *masculine[] = {
			"Bartholomew Roberts", "Benjamin Hornigold", "Blackbeard",
			"Calico Jack", "Charles Vane", "Cheung Po Tsai",
			"Edward England", "Edward Low", "Henry Every",
			"Howell Davis", "Paulsgrave Williams",
			"Samuel Bellamy", "Stede Bonnet", "Thomas Tew",
			"Turgut Reis", "William Kidd", "Emanuel Wynn",
			"Peter Easton", "Richard Worley", "Christopher Contend",
			"Christopher Moody"
		};
		const char *feminine[] = {
			"Anne Bonny", "Grace O'Malley", "Mary Read", "Sayyida al Hurra",
			"Ching Shih", "Jeanne de Clisson", "Elise Eskilsdotter",
			"Mary Wolverston", "Dorothy Monk", "Elizabeth Patrickson",
			"Neel Cuyper", "Ingela Gathenhielm", "Maria Lindsey",
			"Mary Critchett"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_RANGER:{
		//Off Aragorn's geneology. Only slightly a joke.
		const char *masculine[] = {
			"Arathorn","Arador","Argonui","Arassuil","Arahad","Aravorn",
			"Aragost", "Araglas", "Aravir", "Aranuir", "Arahael", "Aranarth",
			"Arvedui","Araphant","Araval","Arveleg","Argeleb","Arvegil","Araphor"
		};
		const char *feminine[] = {
			"Gilraen","Ivorwen","Firiel","Silmarien","Nuneth","Erendis",
			"Ailinel","Almiel","Ancalime"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_ROGUE:{
		//Lankhmar. Can't find a good name list :(
		const char *masculine[] = {
			"Essidinix", "Vellix", "Hringorl", "Hor", "Harrax",
			"Hrey", "Zax", "Effendrit", "Fissif", "Slevyas", "Krovas"
		};
		const char *feminine[] = {
			"Mor", "Vlana", "Mara", "Ivrian", "Slivikin", "Alyx"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_SAMURAI:{
		//h.t. wikipedia
		const char *masculine[] = {
			"Masakatsu", "Kagemori", "Terukage", "Mitsuhide", "Nobutomo",
			"Murashige", "Yoshikage", "Narimasa", "Motochika",
			"Ujisato", "Hideharu", "Tokimune", "Naotaka",
			"Katsumoto", "Naozane", "Okimoto", "Nobutomo",
			"Kiyomori", "Hideyori", "Sadamitsu", "Keisuke"
		};
		const char *feminine[] = {
			"Kaihime", "Ginchiyo", "Tomoe", "Hangaku", "Tomiko",
			"Yodo-dono", "Onamihime", "Teruko", "Tsuruhime",
			"Rui", "Koto"
		};
		const char *family[] = {
			"Genji", "Abe", "Ashina", "Bito", "Chiba", "Date", "Fuji", "Goto",
			"Hachisuka", "Ichijo", "Kamiizumi", "Maeda", "Nagao", "Oda",
			"Rokkaku", "Suda", "Todo", "Ukita", "Wakiya", "Yagyu"
		};
		sprintf(nam, "%s %s", ROLL_FROM(family), fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_TOURIST:{
		//Diskworld
		const char *masculine[] = {
			"Bergholt Stuttley", "Hodgesaargh", "Trevor", "Tomjon", "Victor"
		};
		const char *feminine[] = {
			"Christine","Juliet","Ginger"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_UNDEAD_HUNTER:{
		//Bloodborne, Castlevania, Buffy, Brotherhood of the Wolf
		const char *masculine[] = {
			"Alfred", "Brador", "Djura", "Gascoigne", "Gilbert", "Henryk", "Yamamura",
			"Simon", "Valtr", "Damian", "Wallar", "Olek", "Henryk", "Antal", "Vitus",
			"Jozef",
			"Grant", "Hector", "Maxim", "Albus", "Nathan", "Cornell", "Henry", "Eric",
			"Jonathan",
			"Xander","Rupert",
			"Gregoire", "Jean-Francois", "Thomas", "Mani", "Henri", "Antoine",
		};
		const char *feminine[] = {
			"Adella", "Eileen", "Iosefka", "Adeline", "Henriet", 
			"Sypha", "Maria", "Shanoa", "Carrie", "Stella", "Loretta",
			"Charlotte", "Yoko", "Mina",
			"Willow", "Cordelia", "Anya", "Dawn", "Tara",
			"Marianne", "Sylvia", "Genevieve",
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	case PM_VALKYRIE:
	case PM_AWAKENED_VALKYRIE:
	case PM_TRANSCENDENT_VALKYRIE:{
		const char *feminine[] = {
			"Brynhildr", "Eir", "Geirahod", "Geiravor", "Geirdriful",
			"Geironul", "Geirskogul", "Goll", "Gondul", "Gunnr",
			"Herfjotur", "Herja", "Hladgudr svanhvit", "Hildr", "Hjalmthrimul",
			"Hervor alvitr", "Hjorthrimul", "Hlokk", "Hrist", "Hrund",
			"Kara", "Mist", "Olrun", "Randgrid", "Raogrior",
			"Reginleif", "Rota", "Sanngridr", "Sigrdrifa", "Sigrun",
			"Skalmold", "Skeggjold", "Skogul", "Skuld", "Sveid",
			"Svipul", "Thogn", "Skogul", "Thrima", "Thrudr",
		};
		sprintf(nam, "%s", ROLL_FROM(feminine));
	}break;
	case PM_WORM_THAT_WALKS:{
		const char *names[] = {
			"Charnel Clay", "Myriad", "One-from-many", "Gravesoil", "Mind-gnawer"
			"Nethermost", "Phagos", "Teeming Mind","Waxen",
			
			"Swarmsoul", "Creepclad", "Verminous Unity", "Creepcluster"
		};
		sprintf(nam, "%s", ROLL_FROM(names));
	}break;
	case PM_WIZARD:{
		//Historical and mythical wizards, various web sources
		const char *masculine[] = {
			"Aleister", "Atlantes", "Cagliostro", "Cornelius", "Dee", "Eliphas", "Faust",
			"Loew", "Prospero", "Seimei", "Trismegistus", "Vainamoinen"
		};
		const char *feminine[] = {
			"Bradamante", "Circe", "Endor", "Medea", "Melissa", "Morgan",
			"Sycorax"
		};
		sprintf(nam, "%s", fmlkind ? ROLL_FROM(feminine) : ROLL_FROM(masculine));
	}break;
	default:
		pline("Received %d.",monsndx(mtmp->data));
		impossible("bad mplayer monster in culture name");
		break;
	}
}

STATIC_OVL void
mk_mplayer_armor(mon, typ)
struct monst *mon;
int typ;
{
	struct obj *obj;

	if (typ == STRANGE_OBJECT) return;
	obj = mksobj(typ, MKOBJ_NOINIT);
	if (!rn2(3)) obj->oerodeproof = 1;
	else if (!rn2(2)) obj->greased = 1;
	if (!rn2(3)) curse(obj);
	if (!rn2(3)) bless(obj);
	/* Most players who get to the endgame who have cursed equipment
	 * have it because the wizard or other monsters cursed it, so its
	 * chances of having plusses is the same as usual....
	 */
	obj->spe = rn2(10) ? (rn2(3) ? rn2(5) : rn1(4,4)) : -rnd(3);
	(void) mpickobj(mon, obj);
}

void
init_mplayer_gear(ptr, female, special, weapon, secweapon, rweapon, rwammo, armor, shirt, cloak, helm, boots, gloves, shield, tool)
register struct permonst *ptr;
boolean female;
boolean special;
int *weapon, *secweapon, *rweapon, *rwammo, *armor, *shirt, *cloak, *helm, *boots, *gloves, *shield, *tool;
{
	//Default to nothing
	switch(ptr->mtyp) {
	case PM_ARCHEOLOGIST:
		*weapon = BULLWHIP;
		*armor = JACKET;
		*helm = FEDORA;
		*boots = HIGH_BOOTS;
		*tool = PICK_AXE;
	break;
	case PM_ANACHRONONAUT:
		if(special){
			if (!rn2(2)) *weapon = LIGHTSABER;
			else *weapon = FORCE_PIKE;
			*rweapon = ARM_BLASTER;
			*rwammo = FRAG_GRENADE;
			*shield = CRYSTAL_SHIELD;
			*armor = CRYSTAL_PLATE_MAIL;
			*shirt = BODYGLOVE;
			*helm = CRYSTAL_HELM;
			*gloves = CRYSTAL_GAUNTLETS;
			*boots = CRYSTAL_BOOTS;
			*cloak = CLOAK_OF_MAGIC_RESISTANCE;
		} else {
			if (!rn2(4)) *weapon = FORCE_PIKE;
			else *weapon = VIBROBLADE;
			*rweapon = ASSAULT_RIFLE;
			*rwammo = BULLET;
			*shield = CRYSTAL_SHIELD;
			*armor = PLASTEEL_ARMOR;
			*helm = FLACK_HELMET;
			*gloves = PLASTEEL_GAUNTLETS;
			*boots = HIGH_BOOTS;
		}
	break;
	case PM_BARBARIAN:
	case PM_HALF_DRAGON:{
		if (rn2(2)) {
			*weapon = rn2(2) ? TWO_HANDED_SWORD : BATTLE_AXE;
		}
		*helm = HELMET;
		*armor = CHAIN_MAIL;
		*boots = GLOVES;
		*boots = HIGH_BOOTS;
		*tool = TORCH;
	}break;
	case PM_BARD:{
		static int trotyp[] = {
			FLUTE, TOOLED_HORN, HARP,
			BELL, BUGLE, DRUM
		};
		if(special)
			*weapon = LONG_SWORD;
		*tool = trotyp[rn2(SIZE(trotyp))];
		*armor = rn2(2) ? ELVEN_MITHRIL_COAT : ELVEN_TOGA;
		*cloak = rn2(2) ? DWARVISH_CLOAK : CLOAK;
		*boots = HIGH_BOOTS;
	}break;
	case PM_CAVEMAN:
	case PM_CAVEWOMAN:
		if (rn2(4)) *weapon = MACE;
		else *weapon = CLUB;
		if(special){
			*cloak = LEO_NEMAEUS_HIDE;
		} else {
			*cloak = CLOAK;
			*rweapon = FLINT;
			*rwammo = ROCK;
		}
	break;
#ifdef PM_CONVICT
	case PM_CONVICT:
		if (rn2(4)) *weapon = FLAIL;
		else if(rn2(3)) *weapon = HEAVY_IRON_BALL;
		else *weapon = SPOON;
		if(special){
			*helm = find_vhelm();
			*cloak = find_cope();
			// OBJECT:'#',"blessed iron +2 magic-resistant visored helmet named Mask of Waterdeep",contained
			// OBJECT:'#',"blessed +2 ornamental cope named Masked Lord's Cope",contained
		} else {
			*shirt = STRIPED_SHIRT;
		}
	break;
#endif
	case PM_EXILE:
		*weapon = SCYTHE;
		if(!special){
			*rweapon = SLING;
			*rwammo = ROCK;
			*cloak = CLOAK;
		}
	break;
	case PM_HEALER:
		if (rn2(4)){
			*weapon = QUARTERSTAFF;
		} else if (rn2(2)) *weapon = rn2(2) ? UNICORN_HORN : SCALPEL;
		if (special && rn2(4)) *helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
		*armor = HEALER_UNIFORM;
		*boots = LOW_BOOTS;
		*tool = POT_EXTRA_HEALING;
	break;
	case PM_INCANTIFIER:
		if(Infuture){
			*weapon = rn2(2) ? DOUBLE_LIGHTSABER : LIGHTSABER;
			*rweapon = HAND_BLASTER;
			*armor = !rn2(3) ? JUMPSUIT : 
					 rn2(2) ? ELVEN_TOGA :
					 PLASTEEL_ARMOR;
			*shirt = BODYGLOVE;
			*helm = rn2(2) ? HELM_OF_BRILLIANCE : CRYSTAL_HELM;
			*cloak = rn2(4) ? ROBE : CLOAK_OF_PROTECTION;
		} else {
			if (rn2(4)) *weapon = rn2(2) ? QUARTERSTAFF : ATHAME;
			if(special){
				*armor = rn2(2) ? BLACK_DRAGON_SCALE_MAIL :
						SILVER_DRAGON_SCALE_MAIL;
			}
			*cloak = ROBE;
			if (rn2(4)) *helm = HELM_OF_BRILLIANCE;
		}
	break;
	case PM_KNIGHT:
		*weapon = LONG_SWORD;
		*rweapon = LANCE;
		*shield = KITE_SHIELD;
		*helm = HELMET;
		*armor = PLATE_MAIL;
		*gloves = GAUNTLETS;
		*boots = ARMORED_BOOTS;
	break;
	case PM_MONK:
		*helm = SEDGE_HAT;
		*gloves = GLOVES;
		*boots = LOW_BOOTS;
		*cloak = ROBE;
	break;
	case PM_MADMAN:
		if(special){
			*weapon = RAKUYO;
			*shield = ROUNDSHIELD;
			*armor = GENTLEMAN_S_SUIT;
			*shirt = RUFFLED_SHIRT;
			*cloak = find_opera_cloak();
			*gloves = GLOVES;
			*boots = ARMORED_BOOTS;
			break;
		}
	case PM_MADWOMAN:
		if(special){
			if(!rn2(4)){
				*weapon = RAKUYO_SABER;
				*secweapon = RAKUYO_DAGGER;
				//Loch shield+katana
			}
			else {
				*weapon = BLADE_OF_GRACE;
				*secweapon = BLADE_OF_PITY;
			}
			*armor = GENTLEWOMAN_S_DRESS;
			*shirt = VICTORIAN_UNDERWEAR;
			*cloak = ALCHEMY_SMOCK;
			*gloves = GLOVES;
			*boots = STILETTOS;
			break;
		}
		//Else
		if (rn2(4)) *weapon = KNIFE;
		else if(rn2(3)) *weapon = AXE;
		else *weapon = BULLWHIP;
		*armor = STRAITJACKET;
	break;
	case PM_NOBLEMAN:
		*weapon = special ? LONG_SWORD : RAPIER;
		*armor = special ? CRYSTAL_PLATE_MAIL : GENTLEMAN_S_SUIT;
		*shirt = RUFFLED_SHIRT;
		*cloak = special ? find_opera_cloak() : CLOAK;
		*gloves = special ? CRYSTAL_GAUNTLETS : GLOVES;
		*boots = special ? CRYSTAL_BOOTS : HIGH_BOOTS;
	break;
	case PM_NOBLEWOMAN:
		*weapon = special ? RAKUYO : RAPIER;
		*armor = special ? NOBLE_S_DRESS : GENTLEWOMAN_S_DRESS;
		*shirt = special ? PLAIN_DRESS : VICTORIAN_UNDERWEAR;
		*cloak = special ? find_opera_cloak() : CLOAK;
		*gloves = GLOVES;
		*boots = STILETTOS;
	break;
	case PM_PRIEST:
	case PM_PRIESTESS:
	case PM_ITINERANT_PRIESTESS:
		if(Role_if(PM_MADMAN) && In_quest(&u.uz) && ptr->mtyp == PM_ITINERANT_PRIESTESS){
			*armor = STRAITJACKET;
			*cloak = ROBE;
		}
		else {
			*weapon = MACE;
			if (special && rn2(2)) *armor = PLATE_MAIL;
			*cloak = ROBE;
			if (special && rn2(4)) *helm = rn2(2) ? HELM_OF_BRILLIANCE : HELM_OF_TELEPATHY;
			if (rn2(2)) *shield = BUCKLER;
			*boots = LOW_BOOTS;
			*tool = POT_WATER;
		}
	break;
	case PM_PIRATE:
		*weapon = SCIMITAR;
		*armor = JACKET;
		*shield = BUCKLER;
		*rweapon = FLINTLOCK;
		*rwammo = BULLET;
		*gloves = GLOVES;
		*boots = HIGH_BOOTS;
	break;
	case PM_RANGER:
		*weapon = LONG_SWORD;
		*armor = LEATHER_ARMOR;
		*helm = LEATHER_HELM;
		*rweapon = BOW;
		*rwammo = special ? SILVER_ARROW : ARROW;
		*gloves = GLOVES;
		*boots = HIGH_BOOTS;
	break;
	case PM_ROGUE:
		*weapon = SHORT_SWORD;
		*secweapon = STILETTO;
		*rwammo = DAGGER;
		*helm = LEATHER_HELM;
		*armor = STUDDED_LEATHER_ARMOR;
		*gloves = GLOVES;
		*boots = LOW_BOOTS;
	break;
	case PM_SAMURAI:
		if(female){
			*weapon = NAGINATA;
			*secweapon = KNIFE;
		}
		else {
			*weapon = KATANA;
			*secweapon = WAKIZASHI;
		}
		*rweapon = YUMI;
		*rwammo = YA;
		*helm = HELMET;
		*armor = SPLINT_MAIL;
		*gloves = GAUNTLETS;
		*boots = ARMORED_BOOTS;
		*tool = MASK;
	break;
#ifdef TOURIST
	case PM_TOURIST:
		if (special && !rn2(4)) *weapon = LIGHTSABER;
		*rweapon = EXPENSIVE_CAMERA;
		*rwammo = DART;
		*shirt = HAWAIIAN_SHIRT;
		if(!special) *armor = HAWAIIAN_SHORTS;
		*boots = LOW_BOOTS;
		*tool = CREDIT_CARD;
	break;
#endif
	case PM_UNDEAD_HUNTER:
		switch(rn2(rn2(15))){
			case 0:
				*weapon = WHIP_SAW;
			break;
			case 1:
				*weapon = CANE;
			break;
			case 2:
				*weapon = SOLDIER_S_RAPIER;
			break;
			case 3:
				*weapon = SOLDIER_S_SABER;
			break;
			case 4:
				*weapon = CHIKAGE;
			break;
			case 5:
				*weapon = RAKUYO;
			break;
			case 6:
				*weapon = RAKUYO_SABER;
				*secweapon = RAKUYO_DAGGER;
			break;
			case 7:
				*weapon = CHURCH_HAMMER;
			break;
			case 8:
				*weapon = HUNTER_S_SHORTSWORD;
			break;
			case 9:
				*weapon = CHURCH_BLADE;
			break;
			case 10:
				*weapon = HUNTER_S_LONGSWORD;
			break;
			case 11:
				*weapon = HUNTER_S_AXE;
			break;
			case 12:
				*weapon = HUNTER_S_LONG_AXE;
			break;
			case 13:
				*weapon = SAW_CLEAVER;
			break;
			case 14:
				*weapon = RAZOR_CLEAVER;
			break;
		}
		*armor = JACKET;
		*helm = FEDORA;
		*cloak = CLOAK;
		*gloves = GLOVES;
		*boots = HIGH_BOOTS;
	break;
	case PM_VALKYRIE:
	case PM_AWAKENED_VALKYRIE:
	case PM_TRANSCENDENT_VALKYRIE:
		if (rn2(2)) *weapon = WAR_HAMMER;
		else *weapon = ptr->mtyp == PM_VALKYRIE ? SPEAR : ATGEIR;
		*rweapon = BOW;
		*rwammo = ARROW;
		if(special){
			*helm = HELMET;
			*armor = PLATE_MAIL;
			*gloves = GAUNTLETS;
			*boots = ARMORED_BOOTS;
			*shield = KITE_SHIELD;
		} else {
			*armor = LEATHER_ARMOR;
			*gloves = GLOVES;
			*boots = HIGH_BOOTS;
			*shield = BUCKLER;
		}
	break;
	case PM_WORM_THAT_WALKS:
		*weapon = ATHAME;
		*armor = ARCHAIC_PLATE_MAIL;
		*cloak = MUMMY_WRAPPING;
		*shield = SHIELD_OF_REFLECTION;
		*boots = ARMORED_BOOTS;
	break;
	case PM_WIZARD:
	   *weapon = rn2(2) ? QUARTERSTAFF : ATHAME;
		if (special && rn2(2)) {
			*armor = rn2(2) ? BLACK_DRAGON_SCALE_MAIL :
					SILVER_DRAGON_SCALE_MAIL;
		}
		*cloak = special ? CLOAK_OF_MAGIC_RESISTANCE : CLOAK;
		if(special){
			if (rn2(4)) *helm = rn2(2) ? CORNUTHAUM : HELM_OF_BRILLIANCE;
		}
	break;
	default:
		pline("Received %d.",monsndx(ptr));
		impossible("bad mplayer monster");
		weapon = 0;
		break;
	}
}


struct monst *
mk_mplayer(ptr, x, y, flags)
struct permonst *ptr;
xchar x, y;
long flags;
{
	boolean special = (flags&MM_GOODEQUIP) == MM_GOODEQUIP;
	boolean endgame = (flags&MM_ENDGEQUIP) == MM_ENDGEQUIP;
	struct monst *mtmp;
	char nam[PL_NSIZ] = {0};

	flags &= ~(MM_GOODEQUIP|MM_ENDGEQUIP);
	flags |= NO_MINVENT;

	//if ptr is null don't make a monster
	if(!ptr)
		return((struct monst *)0);


	if(!is_mplayer(ptr))
		return((struct monst *)0);

	if(MON_AT(x, y) && !(flags&MM_ADJACENTOK))
		(void) rloc(m_at(x, y), FALSE); /* insurance */

	if ((mtmp = makemon(ptr, x, y, flags)) != 0) {
	    int quan;
	    struct obj *otmp;
		int weapon, secweapon, rweapon, rwammo, armor, shirt, cloak, helm, boots, gloves, shield, tool;
		boolean uh_patient = In_quest(&u.uz) && Role_if(PM_UNDEAD_HUNTER) && in_mklev && (
			mtmp->mtyp == PM_MADMAN
			|| mtmp->mtyp == PM_MADWOMAN
			|| ((mtmp->mtyp == PM_NOBLEMAN || mtmp->mtyp == PM_NOBLEWOMAN) && mtmp->mx > 57));
		//Default to nothing
		weapon = secweapon = rweapon = rwammo = armor = shirt = cloak = helm = boots = gloves = shield = tool = STRANGE_OBJECT;
		if(uh_patient){
			armor = STRAITJACKET;
		}
		else init_mplayer_gear(ptr, mtmp->female, special, &weapon, &secweapon, &rweapon, &rwammo, &armor, &shirt, &cloak, &helm, &boots, &gloves, &shield, &tool);
		
		if(mtmp && ptr->mtyp == PM_INCANTIFIER && Infuture){
			give_mintrinsic(mtmp, TELEPAT);
			give_mintrinsic(mtmp, REGENERATION);
			give_mintrinsic(mtmp, POISON_RES);
		}
		
		if(endgame){
			static int sweptyp[] = {
				CRYSKNIFE, MOON_AXE, BATTLE_AXE, HIGH_ELVEN_WARSWORD,
				SABER, CRYSTAL_SWORD, TWO_HANDED_SWORD,
				KATANA, DWARVISH_MATTOCK, RUNESWORD
			};
			weapon = rn2(2) ? sweptyp[rn2(SIZE(sweptyp))] : weapon ? weapon : LONG_SWORD;
			//Why turn off ranged attacks?
			// rweapon = STRANGE_OBJECT;
			// rwammo = STRANGE_OBJECT;
			armor = rnd_class(GRAY_DRAGON_SCALE_MAIL, YELLOW_DRAGON_SCALE_MAIL);
			cloak = !rn2(8) ? cloak :
					rnd_class(OILSKIN_CLOAK, CLOAK_OF_DISPLACEMENT);
			helm = !rn2(8) ? helm :
					rnd_class(HELMET, HELM_OF_TELEPATHY);
			shield = !rn2(2) ? rnd_class(GRAY_DRAGON_SCALE_SHIELD, YELLOW_DRAGON_SCALE_SHIELD) 
							 : (!rn2(8) ? shield : rnd_class(ELVEN_SHIELD, SHIELD_OF_REFLECTION));
		}

	    mtmp->m_lev = (special ? rn1(16,15) : rnd(16));
	    mtmp->mhp = mtmp->mhpmax = d((int)mtmp->m_lev,10) +
					(special ? (30 + rnd(30)) : 30);
	    if(special) {
	        get_mplname(mtmp, nam, endgame);
	        mtmp = christen_monst(mtmp, nam);
			/* that's why they are "stuck" in the endgame :-) */
			(void)mongets(mtmp, FAKE_AMULET_OF_YENDOR, NO_MKOBJ_FLAGS);
	    }
	    mtmp->mpeaceful = 0;
	    set_malign(mtmp); /* peaceful may have changed again */

	    if (weapon != STRANGE_OBJECT) {
			otmp = mksobj(weapon, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if(otmp->otyp == RAKUYO && special)
				otmp->spe = 10;
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special){
				if(endgame && rn2(2)) otmp = mk_artifact(otmp, A_NONE);
				
				if(!otmp->oartifact){//mk_artifact can fail, esp for odd base items
					if(rn2(2)) otmp = mk_special(otmp);
					else if(rn2(4)) otmp = mk_minor_special(otmp);
				}
			}
			/* mplayers knew better than to overenchant Magicbane */
			if (otmp->oartifact == ART_MAGICBANE)
				otmp->spe = rnd(4);
			(void) mpickobj(mtmp, otmp);
	    }

	    if (secweapon != STRANGE_OBJECT) {
			otmp = mksobj(secweapon, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if(otmp->otyp == RAKUYO && special)
				otmp->spe = 10;
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special){
				if(endgame && rn2(2)) otmp = mk_artifact(otmp, A_NONE);
				
				if(!otmp->oartifact){//mk_artifact can fail, esp for odd base items
					if(rn2(2)) otmp = mk_special(otmp);
					else if(rn2(4)) otmp = mk_minor_special(otmp);
				}
			}
			/* mplayers knew better than to overenchant Magicbane */
			if (otmp->oartifact == ART_MAGICBANE)
				otmp->spe = rnd(4);
			(void) mpickobj(mtmp, otmp);
	    }

	    if (rweapon != STRANGE_OBJECT) {
			otmp = mksobj(rweapon, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if (!rn2(3)) otmp->oerodeproof = 1;
			else if (!rn2(2)) otmp->greased = 1;
			if (special){
				if(endgame && rn2(2)) otmp = mk_artifact(otmp, A_NONE);
				
				if(!otmp->oartifact){//mk_artifact can fail, esp for odd base items
					if(rn2(2)) otmp = mk_special(otmp);
					else if(rn2(4)) otmp = mk_minor_special(otmp);
				}
			}
			(void) mpickobj(mtmp, otmp);
	    }

	    if (rwammo != STRANGE_OBJECT) {
			otmp = mksobj(rwammo, NO_MKOBJ_FLAGS);
			otmp->spe = (special ? rn1(5,4) : rn2(4));
			if (!rn2(3)) otmp->oerodeproof = 1;
			otmp->quan += special ? 20 : 10;
			(void) mpickobj(mtmp, otmp);
	    }

		if (tool != STRANGE_OBJECT) {
			otmp = mksobj(tool, NO_MKOBJ_FLAGS);
			(void) mpickobj(mtmp, otmp);
		}

	    if(special) {
			if (!rn2(10))
				(void) mongets(mtmp, rn2(3) ? LUCKSTONE : LOADSTONE, NO_MKOBJ_FLAGS);
			mk_mplayer_armor(mtmp, armor);
			mk_mplayer_armor(mtmp, shirt);
			mk_mplayer_armor(mtmp, cloak);
			mk_mplayer_armor(mtmp, helm);
			mk_mplayer_armor(mtmp, boots);
			mk_mplayer_armor(mtmp, gloves);
			mk_mplayer_armor(mtmp, shield);
			
			m_dowear(mtmp, TRUE);
			init_mon_wield_item(mtmp);
			m_level_up_intrinsic(mtmp);

			quan = rn2(3) ? rn2(3) : rn2(16);
			while(quan--)
				(void)mongets(mtmp, rnd_class(DILITHIUM_CRYSTAL, JADE), NO_MKOBJ_FLAGS);
			/* To get the gold "right" would mean a player can double his */
			/* gold supply by killing one mplayer.  Not good. */
#ifndef GOLDOBJ
			mtmp->mgold = rn2(1000);
			u.spawnedGold += mtmp->mgold;
#else
			mkmonmoney(mtmp, rn2(1000));
#endif
			quan = rn2(10);
			while(quan--)
				(void) mpickobj(mtmp, mkobj(RANDOM_CLASS, FALSE));
	    } else {
			(void) mongets(mtmp, armor, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, shirt, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, cloak, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, helm, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, helm, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, boots, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, gloves, NO_MKOBJ_FLAGS);
			(void) mongets(mtmp, shield, NO_MKOBJ_FLAGS);
			
			m_dowear(mtmp, TRUE);
			init_mon_wield_item(mtmp);
			m_level_up_intrinsic(mtmp);
		}
		if(!uh_patient){
#define In_plat_tower (dungeon_topology.alt_tower && (Is_arcadiatower2(&u.uz) || Is_arcadiatower3(&u.uz) || Is_arcadiadonjon(&u.uz)))
			quan = In_plat_tower ? 1 : rnd(3);
			while(quan--)
				(void)mongets(mtmp, rnd_offensive_item(mtmp), NO_MKOBJ_FLAGS);
			quan = In_plat_tower ? 1 : rnd(3);
			while(quan--)
				(void)mongets(mtmp, rnd_defensive_item(mtmp), NO_MKOBJ_FLAGS);
			quan = In_plat_tower ? 1 : rnd(3);
			while(quan--)
				(void)mongets(mtmp, rnd_misc_item(mtmp), NO_MKOBJ_FLAGS);
#undef In_plat_tower
		}

		if((special && (mtmp->mtyp == PM_MADMAN || mtmp->mtyp == PM_MADWOMAN))
			|| (Role_if(PM_MADMAN) && In_quest(&u.uz) && (mtmp->mtyp == PM_NOBLEMAN || mtmp->mtyp == PM_NOBLEWOMAN || mtmp->mtyp == PM_HEALER))
		){
			if(mtmp->mtyp == PM_HEALER && in_mklev)
				mongets(mtmp, TREPHINATION_KIT, NO_MKOBJ_FLAGS);
			for(struct obj *otmp = mtmp->minvent; otmp; otmp = otmp->nobj){
				if(is_metallic(otmp)){
					set_material_gm(otmp, GOLD);
				} else if(otmp->owornmask){
					otmp->obj_color = CLR_YELLOW;
				}
			}
		}

		if(Role_if(PM_MADMAN) && In_quest(&u.uz) && mtmp->mtyp == PM_ITINERANT_PRIESTESS){
			mtmp->m_lev = 10;
			mtmp->mhpmax = d(10, 4) + 40;
			mtmp->mhp = mtmp->mhpmax;
			mtmp->m_insight_level = 10;
			set_template(mtmp, MISTWEAVER);
			for(struct obj *otmp = mtmp->minvent; otmp; otmp = otmp->nobj){
				if(otmp->otyp == STRAITJACKET){
					curse(otmp);
				}
				else if(!is_metallic(otmp)){
					otmp->obj_color = CLR_RED;
					otmp->oeroded3 = 1;
				}
			}
			otmp = mongets(mtmp, SHACKLES, NO_MKOBJ_FLAGS);
			if(otmp){
				mtmp->entangled_otyp = SHACKLES;
				mtmp->entangled_oid = otmp->o_id;
			}
		}
		if(uh_patient){
			for(struct obj *otmp = mtmp->minvent; otmp; otmp = otmp->nobj){
				if(otmp->otyp == STRAITJACKET){
					curse(otmp);
				}
			}
			set_mcan(mtmp, TRUE);
		}
	}

	return(mtmp);
}

/* create the indicated number (num) of monster-players,
 * randomly chosen, and in randomly chosen (free) locations
 * on the level.  If "special", the size of num should not
 * be bigger than the number of _non-repeated_ names in the
 * developers array, otherwise a bunch of Adams and Eves will
 * fill up the overflow.
 */
void
create_mplayers(num, special)
register int num;
boolean special;
{
	int pm, x, y;
	struct monst fakemon = {0}, *mtmp;

	while(num) {
		int tryct = 0;

		/* roll for character class */
		pm = PM_ARCHEOLOGIST + rn2(PM_WIZARD - PM_ARCHEOLOGIST + 1);
		fakemon.data = &mons[pm];

		/* roll for an available location */
		do {
		    x = rn1(COLNO-4, 2);
		    y = rnd(ROWNO-2);
		} while(!goodpos(x, y, &fakemon, 0) && tryct++ <= 50);

		/* if pos not found in 50 tries, don't bother to continue */
		if(tryct > 50) return;

		mtmp = mk_mplayer(&mons[pm], (xchar)x, (xchar)y, special ? MM_GOODEQUIP : NO_MM_FLAGS);
		if(mtmp)
			set_template(mtmp, TONGUE_PUPPET);
		num--;
	}
}

void
mplayer_talk(mtmp)
register struct monst *mtmp;
{
	static const char *same_class_msg[3] = {
		"I can't win, and neither will you!",
		"You don't deserve to win!",
		"Mine should be the honor, not yours!",
	},		  *other_class_msg[3] = {
		"The low-life wants to talk, eh?",
		"Fight, scum!",
		"Here is what I have to say!",
	};

	if(mtmp->mpeaceful) return; /* will drop to humanoid talk */

	pline("Talk? -- %s",
		(mtmp->mtyp == urole.malenum ||
		mtmp->mtyp == urole.femalenum) ?
		same_class_msg[rn2(3)] : other_class_msg[rn2(3)]);
}

/*mplayer.c*/
