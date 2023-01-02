#include <algorithm>
#include <ostream>
#include <string>
#include <iterator>
#include <sstream>
#include <vector>
#include <set>
#include <concepts>

// Concepts

template<typename T>
concept ConstIterable = requires(T cont) {
	{ cont.cbegin() } -> std::convertible_to<typename T::const_iterator>;
	{ cont.cend() } -> std::convertible_to<typename T::const_iterator>;
};

template<typename T>
concept Iterable = requires(T cont) {
	{ cont.begin() } -> std::convertible_to<typename T::iterator>;
	{ cont.end() } -> std::convertible_to<typename T::iterator>;
};

template<ConstIterable container, typename type>
bool hasElement(const container& cont, const type& el) {
	if(std::find(cont.begin(), cont.end(), el) == cont.end()) {
		return false;
	} else {
		return true;
	}
}

// Arrays

template<typename T>
class array_2d {
    public:
        array_2d(size_t nx=0, size_t ny=0) {
            this->nx = nx;
            this->ny = ny;
            initialize_internal();
        }
        ~array_2d() {
            if(array != nullptr) {
                delete [] array;
            }
        }
        void init(size_t nx, size_t ny) {
            this->nx = nx;
            this->ny = ny;
            initialize_internal();
        }
        T& assign(size_t i, size_t j) {
            return this->array[j*nx+i];
        }
        T operator()(size_t i, size_t j) const {
            return this->array[j*nx+i];
        }
        void print(std::ostream& out) const {
            for(size_t j = 0; j < ny; ++j) {
                for(size_t i = 0; i < nx; ++i) {
                    out << (*this)(i,j);
                }
                out << std::endl;
            }
        }
    private:
        size_t nx;
        size_t ny;
        T* array;

        void initialize_internal() {
            if((this->nx == 0)||(this->ny == 0)) {
                array = nullptr;
            } else {
                array = new T[nx*ny];
            }
        }
};

template<typename T>
class array_3d {
    public:
        array_3d(size_t nx=0, size_t ny=0, size_t nz = 0) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            initialize_internal();
        }
        ~array_3d() {
            if(array != nullptr) {
                delete [] array;
            }
        }
        void init(size_t nx, size_t ny, size_t nz) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            initialize_internal();
        }
        T& assign(size_t i, size_t j, size_t k) {
            return this->array[k*nx*ny+j*nx+i];
        }
        T operator()(size_t i, size_t j, size_t k) const {
            return this->array[k*nx*ny+j*nx+i];
        }
        void print(std::ostream& out) const {
            for(size_t k = 0; k < nz; ++k) {
                for(size_t j = 0; j < ny; ++j) {
                    for(size_t i = 0; i < nx; ++i) {
                        out << (*this)(i,j);
                    }
                    out << std::endl;
                }
                out << std::endl;
            }
        }
    private:
        size_t nx;
        size_t ny;
        size_t nz;
        T* array;

        void initialize_internal() {
            if((this->nx == 0)||(this->ny == 0)||(this->nz == 0)) {
                array = nullptr;
            } else {
                array = new T[nx*ny*nz];
            }
        }
};

template<class T>
class map_val {
    public:
        map_val() {
            this->value = T();
            this->set = false;
        }
        map_val& operator=(const T& new_val) {
            this->value = new_val;
            this->set = true;
            return *this;
        }
        T value;
        bool set;
};

// Hash utilities

int pair_hash(int x, int y);
long pair_hash_l(long x, long y);

namespace std {
    template<typename T1, typename T2>
    struct hash<std::pair<T1,T2>> {
        size_t operator()(const std::pair<T1,T2>& p) {
            std::hash<T1> hasher1;
            std::hash<T2> hasher2;
            return hasher1(p.first) ^ hasher2(p.second);
        }
    };
}

// split methods

std::vector<std::string> split(const std::string& s, char delim);

// Initial Definitions
template<typename T> class Point;
template<typename T> class Bivector;

template<typename T>
class Point {
    public:
        Point() {
            this->x = 0;
            this->y = 0;
        }
        Point(T x, T y) {
            this->x = x;
            this->y = y;
        }
        Point(const Point& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
        }
        template<typename U>
        Point(const Point<U>& rhs) {
            this->x = (T) rhs.x;
            this->y = (T) rhs.y;
        }
        Point& operator=(const Point& rhs) {
            this->x = rhs.x;
            this->y = rhs.y;
            return *this;
        }
        bool operator==(const Point& rhs) const {
            if ((this->x == rhs.x) && (this->y == rhs.y)) {
                return true;
            } else {
                return false;
            }
        }
        bool operator!=(const Point& rhs) const {
            return !((*this)==rhs);
        }
        Point operator*(float s) const {
            return Point(this->x*s, this->y*s);
        }
        Point operator-(const Point& rhs) const {
            return Point(this->x-rhs.x, this->y-rhs.y);
        }
        Point operator+(const Point& rhs) const {
            return Point(this->x+rhs.x, this->y+rhs.y);
        }
        Bivector<T> wedge(const Point& rhs) const;
        T dist(const Point& rhs) const {
            return std::abs(this->x-rhs.x)+std::abs(this->y-rhs.y);
        }
        T mag() const {
            return std::abs(this->x)+std::abs(this->y);
        }
        T dot(const Point& rhs) const {
            return this->x*rhs.x+this->y*rhs.y;
        }
        std::string str() const {
            std::stringstream ss;
            ss << this->x << "," << this->y;
            return ss.str();
        }
        T x;
        T y;
};

// Define hash for Point so we can use maps and sets with it
namespace std {
    template<typename T>
    struct hash<Point<T>> {
        size_t operator()(const Point<T>& p) const {
            std::hash<std::pair<T, T>> hasher;
            return hasher(std::pair<T, T>(p.x, p.y));
        }
    };
}

template<typename T>
class Bivector {
    public:
        Bivector() {
            this->xy = 0;
        }
        Bivector(T xy) {
            this->xy = xy;
        }
        Bivector(const Bivector& rhs) {
            this->xy = rhs.xy;
        }
        template<typename U>
        Bivector(const Bivector<U>& rhs) {
            this->xy = (T) rhs.xy;
        }
        Bivector& operator=(const Bivector& rhs) {
            this->xy = rhs.xy;
            return *this;
        }
        bool operator==(const Bivector& rhs) const {
            return (this->xy == rhs.xy);
        }
        bool operator!=(const Bivector& rhs) const {
            return !((*this)==rhs);
        }
        Bivector operator*(float s) const {
            return Bivector(this->xy*s);
        }
        Bivector operator-(const Bivector& rhs) const {
            return Bivector(this->xy-rhs.xy);
        }
        Bivector operator+(const Bivector& rhs) const {
            return Bivector(this->xy+rhs.xy);
        }
        T mag() const {
            return std::abs(this->xy);
        }
        T dot(const Bivector& rhs) const {
            return this->xy*rhs.xy;
        }
        std::string str() const {
            std::stringstream ss;
            ss << this->xy << "i";
            return ss.str();
        }
        T xy;
};

// parts of Point/Bivector which reference eachother
template<typename T>
Bivector<T> Point<T>::wedge(const Point& rhs) const {
    return this->x*rhs.y-this->y*rhs.x;
};

// Needed utilities for working with sets/vectors

template<typename T>
std::vector<T> copy_set_to_vector(const std::set<T>& in_cont) {
    std::vector<T> result;
    for (const T& val: in_cont) {
        result.push_back(val);
    }
    return result;
}

template<std::equality_comparable T>
std::set<T> set_diff_slow(const std::set<T>& A, const std::set<T>& B) {
   std::set<T> R;
   for(const auto& a: A) {
       if (!B.contains(a)) {
           R.insert(a);
       }
   }
   return R;
}

template<std::totally_ordered T>
std::set<T> set_diff_fast(const std::set<T>& A, const std::set<T>& B) {
    // Currently gives segfault?
    std::vector<T> A_v = copy_set_to_vector(A);
    std::sort(A_v.begin(), A_v.end());
    std::vector<T> B_v = copy_set_to_vector(B);
    std::sort(B_v.begin(), B_v.end());
    std::vector<T> R_v;
    std::set_difference(
        A_v.cbegin(), A_v.cend(),
        B_v.cbegin(), B_v.cend(),
        std::back_inserter(R_v)); // The back inserter is needed here, otherwise we get a segfault
    std::set<T> R;
    for (const auto& v: R_v) {
        R.insert(v);
    }
    return R;
}
