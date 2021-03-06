#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

typedef enum ObjectKind
{
	Useless, // useless area such as area in Nut.
	UnknownKind, 
	Screw, //��˿
	Nut, //��ĸ
	HexKey, //�����ǰ���
	Coin //Ӳ��
} ObjectKind;

namespace im {
	int im(BYTE *buff, LONG Width, LONG Height);

	class Rectangle
	{
	public:
		Rectangle() : max_x(0), max_y(0), min_y(0), min_x(0) {}

		bool is_valid(LONG w, LONG h)
		{
			LONG x = max_x - min_x;
			LONG y = max_y - min_y;
			if (x > w*0.9 || y > h * 0.9)
				return false;
			if (x < w / 50 || y < w / 50)
				return false;
			if (x*y > w*h/2)
				return false;
			return true;
		}

		friend std::ostream& operator<<(std::ostream& os, const Rectangle& rec) {
			os << "x : " <<
				rec.max_x << "/" <<
				rec.min_x << "\ty : " <<
				rec.max_y << "/" <<
				rec.min_y;
			return os;
		}

		LONG max_x;
		LONG max_y;
		LONG min_x;
		LONG min_y;
	};

	class ObjectBuff
	{
	public:
		ObjectBuff(const LONG w, const LONG h) : Width(w), Height(h) {
			buff = new BYTE[(w+1)*(h+1)];
		};
		ObjectBuff(const ObjectBuff &ob) : Width(ob.Width), Height(ob.Height) {
			this->buff = new BYTE[(ob.Width+1)*(ob.Height+1)];
		}
		~ObjectBuff() {
			if (buff)
				delete[] buff;
		}
		/* return pixel point of buff[y][x]. 
		meanless x, y will return nullptr.
		0 <= x, y <= Width, Height.
		*/
		BYTE *pixel(LONG x, LONG y) {
			if (x < 0 || y < 0)
				return nullptr;
			if (x > Width || y > Height)
				return nullptr;
			BYTE *row = &buff[y*Width];
			return &row[x];
		}
		/* return pixel point of buff[n].
		meanless x, y will return nullptr.
		0 <= n <= N (Width + 1)*(Height + 1) - 1).
		*/
		BYTE *pixel(LONG n) {
			if (n >= 0 && n <= (Width + 1)*(Height + 1) - 1)
				return buff + n;
			else
				return nullptr;
		}
		const LONG Width; // the Width of the Object image. one row have Width+1 pixels.
		const LONG Height; // the Height of the Object image. one colum have Height+1 pixels.
	private:
		BYTE * buff;
	};

	class Object
	{
	public:
		Object(int * const lbuff, const Rectangle r, const int lable, const LONG w, const LONG h) :
			kind(UnknownKind), lbuff(lbuff),  rec(r), lable(lable), w(w), h(h),
			obuff(r.max_x - r.min_x, r.max_y - r.min_y) {};
		
		ObjectKind kind; // the kind of the Object. value can only from enum ObjectKind.
		ObjectBuff obuff; // store Object data.
		const Rectangle rec; 
		const int lable; // lable value in lable_image.

		/* generate Bit image from lable_image. 
		 Only the pixel whose value is the same with lable will be set 255 
		 and others will be 0.*/
		void make_bit_image() {
			for (LONG r = rec.min_y; r < rec.max_y; r++) {
				int * row = &lbuff[r*w];
				for (LONG c = rec.min_x; c < rec.max_x; c++) {
					*obuff.pixel(c - rec.min_x, r - rec.min_y) = (row[c] == lable) ? 255: 0;
				}
			}
		}

	private:
		int * const lbuff; // lable_image buff.
		const LONG w; // Width of the lable_image.
		const LONG h; // Height of the lable_image.
	};
}