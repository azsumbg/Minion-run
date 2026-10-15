#pragma once

#ifdef PLATSERV_EXPORTS
#define PLATSERV_API __declspec(dllexport)
#else
#define PLATSERV_API __declspec(dllimport)
#endif

#include <random>
#include <iterator>
#include <d2d1.h>

#pragma comment(lib, "d2d1.lib")

constexpr int BAD_PTR{ 5001 };
constexpr int BAD_INDEX{ 5002 };
constexpr int BAD_PARAM{ 5003 };
constexpr int BAD_ERR{ 5004 };
constexpr int BAD_ITERATOR{ 5005 };

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 750.0f };

constexpr float gravity{ 3.0f };
constexpr float linear{ 1.2f };
constexpr float max_jump{ 100.0f };

constexpr D2D1_RECT_F FULL_SCREEN{ 0, 0, scr_width, scr_height };
constexpr D2D1_RECT_F GAME_SCREEN{ 0, sky, scr_width, ground };

enum class fields {
	background1 = 0, background2 = 1, intro = 2, pause = 3, castle = 4, ground = 5, platform1 = 6,
	platform2 = 7, platform3 = 8, cloud1 = 9, cloud2 = 10, cloud3 = 11
};
enum class dirs { stop = 0, left = 1, right = 2, up = 3, down = 4 };
enum class assets { chest = 0, open_chest = 1, life = 2, acid = 3, armor = 4 };
enum class states { stop = 0, walk = 1, jump = 2, fall = 3, attack = 4 };
enum class evils { angry = 0, caveman = 1, zombie = 2 };

namespace indll
{
	struct PLATSERV_API FADING
	{
	private:
		float current_state{ 1.0f };
		int delay{ 80 };
		int max_delay{ 80 };

	public:
		assets type{ assets::chest };
		D2D1_RECT_F position{};

		float get_state()
		{
			--delay;
			if (delay <= 0)
			{
				delay = max_delay;
				current_state -= 0.1f;
			}

			return current_state;
		}
	};

	class PLATSERV_API EXCEPTION
	{
	private:
		int what{ 0 };

	public:
		EXCEPTION(int what_happened);

		const wchar_t* eGet() const;
	};

	class PLATSERV_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:

		RANDIT();

		~RANDIT();

		int operator()(int min, int max);

		float operator()(float min, float max);
	};

	template<typename T> class BAG
	{
	private:
		T* mPtr{ nullptr };
		size_t max_size{ 1 };
		size_t next_pos{ 0 };

	public:
		BAG()
		{
			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}
		BAG(size_t lenght)
		{
			max_size = lenght;

			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}
		BAG(BAG& other)
		{
			if (!other.mPtr)
			{
				max_size = 1;
				next_pos = 0;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			}
			else if (other.mPtr == mPtr)throw EXCEPTION(BAD_PARAM);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (!mPtr)throw EXCEPTION(BAD_PTR);
				else
				{
					for (size_t index = 0; index < next_pos; ++index)mPtr[index] = other.mPtr[index];
				}
			}
		}
		BAG(BAG&& other)
		{
			if (!other.mPtr)
			{
				max_size = 1;
				next_pos = 0;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			}
			else if (other.mPtr == mPtr)throw EXCEPTION(BAD_PARAM);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = other.mPtr;

				other.mPtr = nullptr;
			}
		}

		~BAG()
		{
			free(mPtr);
		}

		BAG& operator = (BAG& other)
		{
			if (!other.mPtr)
			{
				max_size = 1;
				next_pos = 0;

				free(mPtr);

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			}
			else if (other.mPtr == mPtr)throw EXCEPTION(BAD_PARAM);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;

				free(mPtr);

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (!mPtr)throw EXCEPTION(BAD_PTR);
				else
				{
					for (size_t index = 0; index < next_pos; ++index)mPtr[index] = other.mPtr[index];
				}
			}

			return(*this);
		}
		BAG& operator = (BAG&& other)
		{
			free(mPtr);

			if (!other.mPtr)
			{
				max_size = 1;
				next_pos = 0;

				mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			}
			else if (other.mPtr == mPtr)throw EXCEPTION(BAD_PARAM);
			else
			{
				max_size = other.max_size;
				next_pos = other.next_pos;

				mPtr = other.mPtr;

				other.mPtr = nullptr;
			}

			return (*this);
		}

		T& operator[] (size_t index)
		{
			if (index >= next_pos)throw EXCEPTION(BAD_INDEX);

			if (!mPtr)throw EXCEPTION(BAD_PTR);
			
			return mPtr[index];
		}

		bool empty() const
		{
			return (next_pos == 0);
		}
		size_t size() const
		{
			return next_pos;
		}
		size_t capacity() const
		{
			return max_size;
		}

		void clear()
		{
			free(mPtr);

			max_size = 1;
			next_pos = 0;

			mPtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
		}

		T& front()
		{
			if (!mPtr) throw EXCEPTION(BAD_PTR);

			return *mPtr;
		}
		T& back()
		{
			if (!mPtr) throw EXCEPTION(BAD_PTR);

			return mPtr[next_pos - 1];
		}

		void push_back(T element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = element;
					++next_pos;
				}
				else
				{
					++max_size;

					T* temp{ reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T))) };
					
					if (!temp)throw EXCEPTION(BAD_PTR);
					else
					{
						mPtr = temp;

						mPtr[next_pos] = element;
						++next_pos;
					}
				}
			}
		}
		void push_back(T* element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					mPtr[next_pos] = *element;
					++next_pos;
				}
				else
				{
					++max_size;

					T* temp{ reinterpret_cast<T*>(realloc(mPtr, max_size * sizeof(T))) };

					if (!temp)throw EXCEPTION(BAD_PTR);
					else
					{
						mPtr = temp;

						mPtr[next_pos] = *element;
						++next_pos;
					}
				}
			}
		}

		void push_front(T element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else if (empty())
			{
				*mPtr = element;
				++next_pos;
			}
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
					*mPtr = element;
					++next_pos;
				}
				else
				{
					++max_size;

					T* temp{ reinterpret_cast<T*>(realloc(mPtr,max_size * sizeof(T))) };

					if(!temp)throw EXCEPTION(BAD_PTR);
					else
					{
						mPtr = temp;

						for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
						*mPtr = element;
						++next_pos;
					}
				}
			}
		}
		void push_front(T* element)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else if (empty())
			{
				*mPtr = *element;
				++next_pos;
			}
			else
			{
				if (next_pos + 1 <= max_size)
				{
					for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
					*mPtr = *element;
					++next_pos;
				}
				else
				{
					++max_size;

					T* temp{ reinterpret_cast<T*>(realloc(mPtr,max_size * sizeof(T))) };

					if (!temp)throw EXCEPTION(BAD_PTR);
					else
					{
						mPtr = temp;

						for (size_t count = next_pos; count > 0; --count)mPtr[count] = mPtr[count - 1];
						*mPtr = *element;
						++next_pos;
					}
				}
			}
		}

		void insert(T element, size_t index)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (index > next_pos)throw EXCEPTION(BAD_INDEX);
				else if (empty())
				{
					*mPtr = element;
					++next_pos;
				}
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t count = next_pos; count > index; --index)mPtr[index] = mPtr[index - 1];

						mPtr[index] = element;
						++next_pos;
					}
					else
					{
						++max_size;

						T* temp{ reinterpret_cast<T*>(realloc(mPtr,max_size * sizeof(T))) };

						if (!temp)throw EXCEPTION(BAD_PTR);
						else
						{
							mPtr = temp;
							
							for (size_t count = next_pos; count > index; --index)mPtr[index] = mPtr[index - 1];

							mPtr[index] = element;
							++next_pos;
						}
					}
				}
			}
		}
		void insert(T* element, size_t index)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (index > next_pos)throw EXCEPTION(BAD_INDEX);
				else if (empty())
				{
					*mPtr = *element;
					++next_pos;
				}
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t count = next_pos; count > index; --index)mPtr[index] = mPtr[index - 1];

						mPtr[index] = *element;
						++next_pos;
					}
					else
					{
						++max_size;

						T* temp{ reinterpret_cast<T*>(realloc(mPtr,max_size * sizeof(T))) };

						if (!temp)throw EXCEPTION(BAD_PTR);
						else
						{
							mPtr = temp;

							for (size_t count = next_pos; count > index; --index)mPtr[index] = mPtr[index - 1];

							mPtr[index] = *element;
							++next_pos;
						}
					}
				}

			}
		}

		void erase(size_t index)
		{
			if (!mPtr)throw EXCEPTION(BAD_PTR);
			else
			{
				if (index >= next_pos)throw EXCEPTION(BAD_INDEX);
				
				for (size_t count = index; count < next_pos - 1; ++count)mPtr[count] = mPtr[count + 1];
				
				--next_pos;
			}
		}

		class iterator
		{
		private:
			T* it_ptr{ nullptr };
			T* container_ptr{ nullptr };

			size_t bag_size{ 0 };

		public:

			friend class BAG<T>;

			using iterator_category = std::contiguous_iterator_tag;
			using difference_type = ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;

			iterator(T* init_ptr, BAG<T>& init_bag) 
			{
				it_ptr = init_ptr;
				container_ptr = init_bag.mPtr;
				bag_size = init_bag.next_pos;
			}

			reference operator *()
			{
				return *it_ptr;
			}

			pointer operator ->()
			{
				return it_ptr;
			}

			friend bool operator == (const iterator& first, const iterator second)
			{
				return first.it_ptr == second.it_ptr;
			}
			friend bool operator != (const iterator& first, const iterator second)
			{
				return first.it_ptr != second.it_ptr;
			}
			friend bool operator < (const iterator & first, const iterator second)
			{
				return first.it_ptr < second.it_ptr;
			}
			friend bool operator > (const iterator& first, const iterator second)
			{
				return first.it_ptr > second.it_ptr;
			}
			friend bool operator <= (const iterator& first, const iterator second)
			{
				return first.it_ptr <= second.it_ptr;
			}
			friend bool operator >= (const iterator& first, const iterator second)
			{
				return first.it_ptr >= second.it_ptr;
			}

			iterator& operator ++()
			{
				if (it_ptr == (container_ptr + bag_size))throw EXCEPTION(BAD_ITERATOR);
				
				++it_ptr;

				return (*this);
			}
			iterator operator ++(int)
			{
				iterator temp{ (*this) };

				++(*this);

				return temp;
			}

			iterator& operator --()
			{
				if (it_ptr < container_ptr)throw EXCEPTION(BAD_ITERATOR);

				--it_ptr;

				return (*this);
			}
			iterator operator --(int)
			{
				iterator temp{ (*this) };

				--(*this);

				return temp;
			}

			iterator& operator + (size_t step)
			{
				if (it_ptr == (container_ptr + bag_size))throw EXCEPTION(BAD_ITERATOR);

				it_ptr += step;

				return (*this);
			}
			iterator& operator - (size_t step)
			{
				if (it_ptr < container_ptr)throw EXCEPTION(BAD_ITERATOR);
				
				it_ptr -= step;

				return (*this);
			}

		};

		iterator begin()
		{
			return iterator(mPtr, (*this));
		}
		iterator end()
		{
			return iterator((mPtr + next_pos), (*this));
		}

		void erase(iterator which_pos)
		{
			for (size_t count = 0; count < next_pos; ++count)
			{
				if (mPtr + count == which_pos.it_ptr)
				{
					erase(count);
					return;
				}
			}

			throw EXCEPTION(BAD_ITERATOR);
		}
	};

	class PLATSERV_API PROTON
	{
	protected:
		float _width{ 1.0f };
		float _height{ 1.0f };

	public:
		D2D1_POINT_2F start{};
		D2D1_POINT_2F end{};
		D2D1_POINT_2F center{};

		D2D1_RECT_F rect{};

		float x_rad{ 0 };
		float y_rad{ 0 };

		PROTON();
		PROTON(float sx, float sy);
		PROTON(D2D1_POINT_2F start_point);
		PROTON(float sx, float sy, float width, float height);
		PROTON(D2D1_POINT_2F start_point, float width, float height);

		virtual ~PROTON() {};

		float get_width() const;
		float get_height() const;

		void set_edges();
		
		void set_width(float new_width);
		void set_height(float new_height);
		void set_dims(float new_width, float new_height);
	};

	class PLATSERV_API FIELD :public PROTON
	{
	private:
		fields type{ fields::background1 };

		int frame{ 0 };
		int max_frames{ 0 };
		int max_frame_delay{ 0 };
		int frame_delay{ 0 };

		FIELD(fields _what_type, float _sx, float _sy);

	public:

		bool move(float gear, dirs to_where);

		fields get_type() const;

		int get_frame();

		void Release();

		static FIELD* create(fields what_type, float sx, float sy);
	};

	class PLATSERV_API SHOT :public PROTON
	{
	private:
		float delta_x{ 0 };
		float delta_y{ 0 };

		float move_sx{ 0 };
		float move_sy{ 0 };

		float move_ex{ 0 };
		float move_ey{ 0 };

		SHOT(float _sx, float _sy, float _ex, float _ey);

	public:
		int damage{ 20 };

		bool move(float gear);

		void Release();

		static SHOT* create(float sx, float sy, float ex, float ey);
	};

	class PLATSERV_API HERO :public PROTON
	{
	private:
		float delta_x{ 0 };
		float delta_y{ 0 };

		int frame{ 0 };
		int max_frames{ 15 };
		int frame_delay{ 4 };
		int max_frame_delay{ 4 };

		float move_sx{ 0 };
		float move_sy{ 0 };
		float move_ex{ 0 };
		float move_ey{ 0 };

		void set_path(float _ex, float _ey);

	public:
		states state = states::stop;
		dirs dir = dirs::right;

		int damage = 20;
		int lifes = 100;
		int armor{ 1 };

		HERO(float sx, float sy);

		int get_frame();

		void move(float gear);
		void jump(float gear);
		void fall(float gear, BAG<D2D1_RECT_F>& fields_bag);
	};

	class PLATSERV_API EVILS :public PROTON
	{
	private:
		evils type{ evils::angry };

		float delta_x{ 0 };
		float delta_y{ 0 };
		float _speed{ linear };

		float move_sx{ 0 };
		float move_ex{ 0 };
		float move_sy{ 0 };
		float move_ey{ 0 };

		int frame{ 0 };
		int max_frames{ 0 };
		int frame_delay{ 0 };
		int max_frame_delay{ 0 };

		int attack_delay{ 0 };
		int max_attack_delay{ 0 };

		EVILS(evils _what, float _sx, float _sy);

	public:
		dirs dir{ dirs::left };
		states state{ states::walk };
		int lifes{ 0 };
		int damage{ 0 };
		int armor{ 1 };

		evils get_type()const;

		void set_path(float _ex, float _ey);

		float get_target_x()const;
		float get_target_y()const;

		int get_frame();
		int attack();

		void move(float gear);
		void jump(float gear);
		void fall(float gear, BAG<D2D1_RECT_F>& fields_bag);

		void Release();

		static EVILS* create(evils what, float sx, float sy);
	};

	// FUNCTIONS ***************************************

	bool PLATSERV_API Intersect(D2D1_RECT_F first, D2D1_RECT_F second);

	float PLATSERV_API Distance(D2D1_POINT_2F first, D2D1_POINT_2F second);

	void PLATSERV_API Sort(BAG<D2D1_POINT_2F>& bag, D2D1_POINT_2F ref);

	void PLATSERV_API EvilNextMove(EVILS& unit, BAG<D2D1_POINT_2F>& bAssetsCenters, BAG<D2D1_RECT_F>& bGrounds, 
		D2D1_POINT_2F hero_center, float level_gear);
}