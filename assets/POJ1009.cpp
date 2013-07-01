#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

class ExtendedRLE {
public:
	static const int INVALID_VALUE = -1;
private:
	const int value;
	const int start;
	const int length;
public:
	ExtendedRLE(int value, int start, int length)
		: value(value), start(start), length(length)
	{}
	ExtendedRLE(const ExtendedRLE &other)
		: value(other.value), start(other.start), length(other.length)
	{}
	ExtendedRLE & operator=(const ExtendedRLE & other) {
		if (this != &other) {
			const_cast<int &>(value) = other.value;
			const_cast<int &>(start) = other.start;
			const_cast<int &>(length) = other.length;
		}
		return *this;
	}
	int GetValue(void) const { return value; }
	int GetStartPosition(void) const { return start; }
	int GetEndPosition(void) const { return start + length; }
	int GetLength(void) const { return length; }
};

class P1009Solver {
private:
	typedef vector<const ExtendedRLE>::const_iterator iterator_type;
	static enum {
		SURROUNDING_LEFT_UP,
		SURROUNDING_MIDDLE_UP,
		SURROUNDING_RIGHT_UP,
		SURROUNDING_LEFT_MIDDLE,
		SURROUNDING_RIGHT_MIDDLE,
		SURROUNDING_LEFT_DOWN,
		SURROUNDING_MIDDLE_DOWN,
		SURROUNDING_RIGHT_DOWN
	};
	static const int NEIGHBOURS_COUNT = 8;
	const int width;
	const iterator_type FAKE_BEGIN_RLE_IT;
	const iterator_type FAKE_BOUNDARY_RLE_IT;
	const iterator_type FAKE_END_RLE_IT;
	const vector<const ExtendedRLE> &pixels;
	const int eoi;
public:
	P1009Solver(int width, vector<const ExtendedRLE> &pixels, int eoi)
		: width(width), pixels(pixels), eoi(eoi)
	{
		pixels.push_back(ExtendedRLE(ExtendedRLE::INVALID_VALUE, -width, width));
		const_cast<iterator_type &>(FAKE_BEGIN_RLE_IT) = pixels.end() - 1;

		pixels.push_back(ExtendedRLE(ExtendedRLE::INVALID_VALUE, 0, 1));
		const_cast<iterator_type &>(FAKE_BOUNDARY_RLE_IT) = pixels.end() - 1;

		pixels.push_back(ExtendedRLE(ExtendedRLE::INVALID_VALUE, eoi, width));
		const_cast<iterator_type &>(FAKE_END_RLE_IT) = pixels.end() - 1;
	}
	vector<pair<int, int> > GetResult(void) const;
private:
	void GetSurroundingIts(int position, iterator_type currentIt, iterator_type surroundingIts[NEIGHBOURS_COUNT]) const;
	int GetMinSameLength(int position, iterator_type currentIt, iterator_type surroundingIts[NEIGHBOURS_COUNT]) const;
	static int CalculateOutput(iterator_type currentIt, iterator_type surroundingIts[NEIGHBOURS_COUNT]);
	bool IsCurrentInRLEMiddle(iterator_type surroundingIts[NEIGHBOURS_COUNT]) const;
	static void PushOutputValue(vector<pair<int, int> > &stack, int value, int length);
private:
	static bool PredByStartPosition(const ExtendedRLE &v1, const ExtendedRLE &v2);
	iterator_type GetUpPositionIt(int position, iterator_type currentIt) const;
	iterator_type GetDownPositionIt(int position, iterator_type currentIt) const;
	iterator_type GetPreviousPositionIt(int position, iterator_type currentIt) const;
	iterator_type GetNextPositionIt(int position, iterator_type currentIt) const;
};

class ToDistanceFunctor {
private:
	const int currentValue;
	static int Distance(int v1, int v2);
public:
	ToDistanceFunctor(int currentValue) : currentValue(currentValue) {}
	int operator () (vector<const ExtendedRLE>::const_iterator it) {
		return Distance(currentValue, it->GetValue());
	}
};

int main(void)
{
	const int MAX_RLE_COUNT = 1000;
	while (true) {
		int width;
		cin >> width;
		if (!width) {
			break;
		}

		int eoi = 0;
		vector<const ExtendedRLE> pixels;
		pixels.reserve(MAX_RLE_COUNT);

		while (true) {
			int v, l;
			cin >> v >> l;
			if (!v && !l) {
				break;
			}
			pixels.push_back(ExtendedRLE(v, eoi, l));
			eoi += l;
		}

		P1009Solver solver(width, pixels, eoi);
		const vector<pair<int, int> > results = solver.GetResult();
		cout << width << endl;
		for (vector<pair<int, int> >::const_iterator it = results.begin();
			it != results.end();
			it++) {
				cout << it->first << " " << it->second << endl;
		}
		cout << "0 0" << endl;
	}
	cout << "0" << endl;
}

vector<pair<int, int> > P1009Solver::GetResult(void) const
{
	const int MAX_RESULT_RLE_COUNT = 1000;
	vector<pair<int, int> > results;
	results.reserve(MAX_RESULT_RLE_COUNT);

	iterator_type currentIt = pixels.begin();
	int sz = pixels.size();
	for (int position = 0; position < eoi;) {
		iterator_type surroundingIts[NEIGHBOURS_COUNT];
		GetSurroundingIts(position, currentIt, surroundingIts);
		int outputValue = CalculateOutput(currentIt, surroundingIts);

		if (IsCurrentInRLEMiddle(surroundingIts)) {
			int l = GetMinSameLength(position, currentIt, surroundingIts);
			if (l == 0) {
				l++;
			}
			PushOutputValue(results, outputValue, l);
			position += l;
		} else {
			PushOutputValue(results, outputValue, 1);
			position++;
		}

		if (position >= currentIt->GetEndPosition()) {
			currentIt++;
		}
	}

	return results;
}

void P1009Solver::GetSurroundingIts(
	int position,
	iterator_type currentIt,
	iterator_type surroundingIts[NEIGHBOURS_COUNT]) const
{
	int midUpPos = position - width;
	iterator_type midUpIt = GetUpPositionIt(position, currentIt);
	surroundingIts[SURROUNDING_LEFT_UP] = GetPreviousPositionIt(midUpPos, midUpIt);
	surroundingIts[SURROUNDING_MIDDLE_UP] = midUpIt;
	surroundingIts[SURROUNDING_RIGHT_UP] = GetNextPositionIt(midUpPos, midUpIt);
	
	surroundingIts[SURROUNDING_LEFT_MIDDLE] = GetPreviousPositionIt(position, currentIt);
	surroundingIts[SURROUNDING_RIGHT_MIDDLE] = GetNextPositionIt(position, currentIt);

	int midDownPos = position + width;
	iterator_type midDownIt = GetDownPositionIt(position, currentIt);
	surroundingIts[SURROUNDING_LEFT_DOWN] = GetPreviousPositionIt(midDownPos, midDownIt);
	surroundingIts[SURROUNDING_MIDDLE_DOWN] = midDownIt;
	surroundingIts[SURROUNDING_RIGHT_DOWN] = GetNextPositionIt(midDownPos, midDownIt);
}

int P1009Solver::GetMinSameLength(
	int position,
	iterator_type currentIt,
	iterator_type surroundingIts[NEIGHBOURS_COUNT]) const
{
	int lengths[3];
	lengths[0] = surroundingIts[SURROUNDING_MIDDLE_UP]->GetEndPosition() - (position - width + 1);
	lengths[1] = currentIt->GetEndPosition() - (position + 1);
	lengths[2] = surroundingIts[SURROUNDING_MIDDLE_DOWN]->GetEndPosition() - (position + width + 1);
	return *min_element(&lengths[0], &lengths[3]);
}

int P1009Solver::CalculateOutput(
	iterator_type currentIt,
	iterator_type surroundingIts[NEIGHBOURS_COUNT])
{
	int values[NEIGHBOURS_COUNT];
	transform(
		&surroundingIts[0],
		&surroundingIts[NEIGHBOURS_COUNT],
		&values[0],
		ToDistanceFunctor(currentIt->GetValue()));
	return *max_element(&values[0], &values[NEIGHBOURS_COUNT]);
}

bool P1009Solver::IsCurrentInRLEMiddle(iterator_type surroundingIts[NEIGHBOURS_COUNT]) const
{
	if (width == 1) {
		return surroundingIts[SURROUNDING_MIDDLE_UP] == surroundingIts[SURROUNDING_MIDDLE_DOWN];
	} else if (width == 2) {
		if (surroundingIts[SURROUNDING_LEFT_MIDDLE] == FAKE_BOUNDARY_RLE_IT) {
			return surroundingIts[SURROUNDING_RIGHT_MIDDLE] == surroundingIts[SURROUNDING_RIGHT_UP]
			&& surroundingIts[SURROUNDING_RIGHT_MIDDLE] == surroundingIts[SURROUNDING_RIGHT_DOWN]
			&& surroundingIts[SURROUNDING_RIGHT_MIDDLE] == surroundingIts[SURROUNDING_MIDDLE_UP]
			&& surroundingIts[SURROUNDING_RIGHT_MIDDLE] == surroundingIts[SURROUNDING_MIDDLE_DOWN];
		} else {
			return surroundingIts[SURROUNDING_LEFT_MIDDLE] == surroundingIts[SURROUNDING_LEFT_UP]
			&& surroundingIts[SURROUNDING_LEFT_MIDDLE] == surroundingIts[SURROUNDING_LEFT_DOWN]
			&& surroundingIts[SURROUNDING_LEFT_MIDDLE] == surroundingIts[SURROUNDING_MIDDLE_UP]
			&& surroundingIts[SURROUNDING_LEFT_MIDDLE] == surroundingIts[SURROUNDING_MIDDLE_DOWN];
		}
	} else {
		return surroundingIts[SURROUNDING_LEFT_MIDDLE] == surroundingIts[SURROUNDING_RIGHT_MIDDLE]
		&& surroundingIts[SURROUNDING_LEFT_UP] == surroundingIts[SURROUNDING_MIDDLE_UP]
		&& surroundingIts[SURROUNDING_LEFT_UP] == surroundingIts[SURROUNDING_RIGHT_UP]
		&& surroundingIts[SURROUNDING_LEFT_DOWN] == surroundingIts[SURROUNDING_MIDDLE_DOWN]
		&& surroundingIts[SURROUNDING_LEFT_DOWN] == surroundingIts[SURROUNDING_RIGHT_DOWN];
	}
}

void P1009Solver::PushOutputValue(vector<pair<int, int> > &stack, int value, int length)
{
	if (!stack.empty() && stack.back().first == value) {
		stack.back().second += length;
	} else {
		stack.push_back(make_pair(value, length));
	}
}

int ToDistanceFunctor::Distance(int v1, int v2)
{
	if (v1 == ExtendedRLE::INVALID_VALUE || v2 == ExtendedRLE::INVALID_VALUE) {
		return 0;
	} else {
		return abs(v1 - v2);
	}
}

bool P1009Solver::PredByStartPosition(const ExtendedRLE &v1, const ExtendedRLE &v2)
{
	return v1.GetStartPosition() < v2.GetStartPosition();
}

P1009Solver::iterator_type P1009Solver::GetUpPositionIt(int position, iterator_type currentIt) const
{
	int midUpPos = position - width;
	if (midUpPos < 0) {
		return FAKE_BEGIN_RLE_IT;
	} else {
		iterator_type it = lower_bound(
			pixels.begin(),
			currentIt,
			ExtendedRLE(ExtendedRLE::INVALID_VALUE, midUpPos, 0),
			PredByStartPosition);
		return it->GetStartPosition() <= midUpPos && it->GetEndPosition() > midUpPos ? it : it - 1;
	}
}

P1009Solver::iterator_type P1009Solver::GetDownPositionIt(int position, iterator_type currentIt) const
{
	int midDownPos = position + width;
	if (midDownPos >= eoi) {
		return FAKE_END_RLE_IT;
	} else {
		iterator_type it = lower_bound(
			currentIt,
			pixels.end() - 3,
			ExtendedRLE(ExtendedRLE::INVALID_VALUE, midDownPos, 0),
			PredByStartPosition);
		return it->GetStartPosition() <= midDownPos && it->GetEndPosition() > midDownPos ? it : it - 1;
	}
}

P1009Solver::iterator_type P1009Solver::GetPreviousPositionIt(int position, iterator_type currentIt) const
{
	if (position % width == 0) {
		return FAKE_BOUNDARY_RLE_IT;
	} else if (currentIt == FAKE_BEGIN_RLE_IT || currentIt == FAKE_END_RLE_IT) {
		return currentIt;
	} else {
		return ((position - 1) < currentIt->GetStartPosition()) ? (currentIt - 1) : currentIt;
	}
}

P1009Solver::iterator_type P1009Solver::GetNextPositionIt(int position, iterator_type currentIt) const
{
	if ((position + 1) % width == 0) {
		return FAKE_BOUNDARY_RLE_IT;
	} else {if (currentIt == FAKE_BEGIN_RLE_IT || currentIt == FAKE_END_RLE_IT) {
		return currentIt;
	} else 
		return ((position + 1) >= currentIt->GetEndPosition()) ? (currentIt + 1) : currentIt;
	}
}

