#include "common/garmin.h"
#include "huffmantable.h"


using namespace IMG;

static inline quint32 readVUint32(const quint8 *buffer, quint32 bytes)
{
	quint32 val = 0;

	for (quint32 i = bytes; i; i--)
		val |= ((quint32)*(buffer + i)) << ((i-1) * 8);

	return val;
}

bool HuffmanTable::load(const RGNFile *rgn, SubFile::Handle &rgnHdl)
{
	if (!_buffer.load(rgn, rgnHdl))
		return false;

	_s0 = (quint8)_buffer.at(0) & 0x0F;
	_s1e = (quint8)_buffer.at(0) & 0x10 ? 1 : 8;
	_s2 = (quint8)_buffer.at(1);
	_s3 = bs(_s2);
	_s1d = (quint8)_buffer.at(2);
	_s1f = (quint8)_buffer.at(3);
	_s20 = bs(_s1f);
	_s1 = _s20 + 1;
	_s22 = vs(_buffer.at(4));
	_s1c = _s3 + 1 + _s22;
	_s14 = (quint8*)(_buffer.data()) + 4 + _s22;
	_s10 = _s14 + _s1c * _s1d;
	_s18 = _s10 + (_s1 << _s0);

	return true;
}

quint32 HuffmanTable::symbol(quint32 data, quint8 &size) const
{
	quint32 ss, sym;
	quint8 *tp;


	if (_s0 == 0) {
		sym = _s1d - 1;
		ss = 0;
	} else {
		quint32 offset = _s1 * (data >> (0x20U - _s0));
		tp = _s10 + offset;

		if ((*tp & 1) != 0) {
			sym = readVUint32(tp, _s20);
			size = *tp >> 1;
			return sym;
		}

		ss = *tp >> 1;
		sym = tp[1];
	}

	tp = ss * _s1c + _s14;
	data = data >> (0x20U - _s2);

	quint8 *prev = tp;
	while (ss < sym) {
		quint32 cnt = (ss + 1 + sym) >> 1;
		tp = _s14 + (cnt * _s1c);
		quint32 nd = readVUint32(tp - 1, _s3);

		if (data <= nd) {
			if (data == nd)
				ss = cnt;
			else
				tp = prev;

			sym = cnt - (data < nd);
			cnt = ss;
		}
		ss = cnt;
		prev = tp;
	}

	sym = readVUint32(tp - 1, _s3);
	tp = tp + _s3;
	ss = readVUint32(tp, _s22);
	size = *tp;
	sym = (data - sym) >> (_s2 - *tp);

	if (_s1e == 8)
		sym = readVUint32(tp, _s20);
	else {
		sym = (sym + ss) * _s1f;
		ss = sym >> 3;
		sym = sym & 7;
		quint32 shift = 8 - sym;
		sym = *(_s18 + ss) >> sym;

		if (shift < _s1f) {
			tp = _s18 + ss;
			ss = readVUint32(tp, ((_s1f + 7) - shift) >> 3);
			sym = (ss << shift) | sym;
		}
	}

	return sym;
}
