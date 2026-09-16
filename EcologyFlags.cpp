#include "EcologyFlags.h"


constexpr std::string_view getEcoName(Ecology flag) {
	switch (flag) {
	case Ecology::gooblet:			return "gooblet";
	case Ecology::gooblet_mate:		return "goobletMate";
	case Ecology::bush:				return "bush";
	case Ecology::pond:				return "pond";
	case Ecology::other_obstacle:	return "obstacle";
	case Ecology::num_flags:		return "ecoFlag";
	}
}
std::ostream& operator<<(std::ostream& out, Ecology flag) {
	out << getEcoName(flag);
	return out;
}
std::ofstream& operator<<(std::ofstream& out, Ecology flag) {
	out << getEcoName(flag);
	return out;
}

constexpr std::string_view getMetaEcoName(MetaEcology flag) {
	switch (flag) {
	case MetaEcology::food:			return "food";
	case MetaEcology::water:		return "water";
	case MetaEcology::mate:			return "mate";
	case MetaEcology::danger:		return "danger";
	case MetaEcology::num_metaflags:return "metaEcoFlag";
	}
}
std::ostream& operator<<(std::ostream& out, MetaEcology flag) {
	out << getMetaEcoName(flag);
	return out;
}
std::ofstream& operator<<(std::ofstream& out, MetaEcology flag) {
	out << getMetaEcoName(flag);
	return out;
}