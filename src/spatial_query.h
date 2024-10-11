
#ifndef SPACE_PARTITION_CLASSES_H
#define SPACE_PARTITION_CLASSES_H

#include <cmath>
#include <vector>
#include <list>
#include <atomic>
#include <functional>
#include <algorithm>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/wrapped.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>


constexpr float float_epsilon = 1e-9;
inline bool float_equal(float a,float b,float epsilon = float_epsilon){
    return (std::fabs(a-b) < epsilon);
}

struct Rect{
    float x = 0;
    float y = 0;
    float height = 0;
    float width = 0;
    inline bool is_inner(float ix,float iy) const{
        if(ix < x)
            return 0;
        if(ix > x + width)
            return 0;
        if(iy < y)
            return 0;
        if(iy > y + height)
            return 0;
        return 1;
    }
    inline bool is_inner(Rect rect) const{
        if(rect.x < x)
            return 0;
        if(rect.x + rect.width > x + width)
            return 0;
        if(rect.y < y)
            return 0;
        if(rect.y + rect.height > y + height)
            return 0;
        return 1;
    }
};
typedef std::vector<std::pair<godot::Variant,godot::Vector2>> lmap;
class SpatialQuery : public godot::RefCounted{
    GDCLASS(SpatialQuery,godot::RefCounted)

    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("set_size", "size"), &SpatialQuery::set_size);
        godot::ClassDB::bind_method(godot::D_METHOD("get_size"), &SpatialQuery::get_size);
        godot::ClassDB::add_property("SpatialQuery", godot::PropertyInfo(godot::Variant::INT, "size"),
                                     "set_size", "get_size");
    }

protected:
    lmap objects;
public:
    Rect bound;
    std::atomic<int> size = 0;
    int get_size(){
        return size;
    }
    void set_size(int s){
        size = s;
    }
    virtual ~SpatialQuery()= default;
    virtual void clear() = 0;
    virtual void set_bound(float x,float y,float height,float width) = 0;
    virtual godot::Array get_all_points() = 0;
    virtual godot::PackedVector2Array get_all_positions() = 0;
    virtual void add_point(godot::Variant value,godot::Vector2 pos) = 0;
    virtual void update_point_pos(int index,godot::Vector2 pos) = 0;
    virtual void update_point_value(int index,godot::Variant value) = 0;
    virtual godot::Variant get_point_at(int index) = 0;
    virtual godot::Vector2 get_position_at(int index) = 0;
    virtual void remove_at(int index) = 0;
    virtual void remove_range(godot::Array indices) = 0;
    virtual bool has_neighbor(godot::Vector2 pos, float radius) = 0;
    virtual godot::Variant get_nearest_point(godot::Vector2 pos) = 0;
    virtual int get_nearest_index(godot::Vector2 pos) = 0;
    virtual godot::PackedInt32Array find_neighbors(godot::Vector2 pos,float radius,int max_count = -1) = 0;
    virtual godot::Array find_neighbor_points(godot::Vector2 pos,float radius,int max_count = -1) = 0;
    virtual void execute(godot::Callable mapper) = 0;
    virtual void custom_sort(godot::Callable sorter) = 0;
    virtual void optimize() = 0;
};
class QuadTreeQuery : public SpatialQuery{
GDCLASS(QuadTreeQuery,SpatialQuery)
    static void _bind_methods(){
        godot::ClassDB::bind_method(godot::D_METHOD("clear"),&QuadTreeQuery::clear);
        godot::ClassDB::bind_method(godot::D_METHOD("set_bound","x","y","height","width"),&QuadTreeQuery::set_bound);
        godot::ClassDB::bind_method(godot::D_METHOD("get_all_rects"),&QuadTreeQuery::get_all_rects);
        godot::ClassDB::bind_method(godot::D_METHOD("get_all_points"),&QuadTreeQuery::get_all_points);
        godot::ClassDB::bind_method(godot::D_METHOD("get_all_positions"),&QuadTreeQuery::get_all_positions);
        godot::ClassDB::bind_method(godot::D_METHOD("add_point","value","pos"),&QuadTreeQuery::add_point);
        godot::ClassDB::bind_method(godot::D_METHOD("update_point_pos","index","pos"),&QuadTreeQuery::update_point_pos);
        godot::ClassDB::bind_method(godot::D_METHOD("update_point_value","index","value"),&QuadTreeQuery::update_point_value);
        godot::ClassDB::bind_method(godot::D_METHOD("get_point_at","index"),&QuadTreeQuery::get_point_at);
        godot::ClassDB::bind_method(godot::D_METHOD("get_position_at","index"),&QuadTreeQuery::get_position_at);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_at","index"),&QuadTreeQuery::remove_at);
        godot::ClassDB::bind_method(godot::D_METHOD("remove_range","indices"),&QuadTreeQuery::remove_range);
        godot::ClassDB::bind_method(godot::D_METHOD("has_neighbor","pos","radius"),&QuadTreeQuery::has_neighbor);
        godot::ClassDB::bind_method(godot::D_METHOD("get_nearest_point","pos"),&QuadTreeQuery::get_nearest_point);
        godot::ClassDB::bind_method(godot::D_METHOD("get_nearest_index","pos"),&QuadTreeQuery::get_nearest_index);
        godot::ClassDB::bind_method(godot::D_METHOD("find_neighbors","pos","radius","max_count"),&QuadTreeQuery::find_neighbors);
        godot::ClassDB::bind_method(godot::D_METHOD("find_neighbor_points","pos","radius","max_count"),&QuadTreeQuery::find_neighbor_points);
        godot::ClassDB::bind_method(godot::D_METHOD("execute","mapper"),&QuadTreeQuery::execute);
        godot::ClassDB::bind_method(godot::D_METHOD("custom_sort","sorter"),&QuadTreeQuery::custom_sort);
        godot::ClassDB::bind_method(godot::D_METHOD("optimize"),&QuadTreeQuery::optimize);


        godot::ClassDB::bind_method(godot::D_METHOD("divide"),&QuadTreeQuery::divide);
    }
public:
    struct QuadTreeNode{
        int depth : 31 = 1;
        bool isDivide : 1 = 0;
        Rect bound;
        const lmap& objects;
        std::array<QuadTreeNode*,4> children{};
        std::list<int> refs;
        explicit QuadTreeNode(const lmap& objects):objects(objects){
            for(auto& a : children){
                a = nullptr;
            }
        }
        QuadTreeNode(int depth,Rect bound,const lmap& objects): depth(depth),bound(bound),objects(objects){
            for(auto& a : children){
                a = nullptr;
            }
        }
        ~QuadTreeNode(){
            if(isDivide){
                for(auto& a : children){
                    delete a;
                    a = nullptr;
                }
                isDivide = 0;
            }
        }
        void divide(){
            if(isDivide){
                for(auto a : children)
                    a->divide();
            }
            isDivide = 1;
            children[0] = new QuadTreeNode(depth+1,{bound.x,bound.y,bound.height/2,bound.width/2},objects);
            children[1] = new QuadTreeNode(depth+1,{bound.x + bound.width/2,bound.y,bound.height/2,bound.width/2},objects);
            children[2] = new QuadTreeNode(depth+1,{bound.x,bound.y+bound.height/2,bound.height/2,bound.width/2},objects);
            children[3] = new QuadTreeNode(depth+1,{bound.x + bound.width/2,bound.y+bound.height/2,bound.height/2,bound.width/2},objects);
            auto beg = refs.begin();
            while(beg != refs.end()){
                const auto& pos =objects[(*beg)].second;
                for(auto b:children){
                    if(b->bound.is_inner(pos.x,pos.y)){
                        b->refs.push_back((*beg));
                        auto temp = beg;
                        ++beg;
                        refs.erase(temp);
                        goto END_OF_WHILE;
                    }
                }
                ++beg;
            END_OF_WHILE:
                ;
            }
        }
        void addRef(int index){
            auto pos = objects[index].second;
//            if(!bound.is_inner(pos.x,pos.y))
//                return;
            if(!isDivide){
                refs.push_back(index);
                return;
            }
            if(pos.y <= bound.y + bound.height/2){
                if(pos.x <= bound.x + bound.width/2)
                    children[0]->addRef(index);
                else
                    children[1]->addRef(index);
            }else{
                if(pos.x <= bound.x + bound.width/2)
                    children[2]->addRef(index);
                else
                    children[3]->addRef(index);
            }
        }
        int find(godot::Vector2 pos){
            if(!isDivide){
                auto size = objects.size();
                for(int i = 0;i!=size;++i){
                    auto& a = objects[i].second;
                    if(float_equal(pos.x,a.x)&& float_equal(pos.y,a.y))
                        return i;
                }
                return -1;
            }
            if(pos.y <= bound.y + bound.height/2){
                if(pos.x <= bound.x + bound.width/2)
                    return children[0]->find(pos);
                else
                    return children[1]->find(pos);
            }else{
                if(pos.x <= bound.x + bound.width/2)
                    return children[2]->find(pos);
                else
                    return children[3]->find(pos);
            }
        }
        QuadTreeNode* findBoundingBox(const Rect& rect){
            if(!bound.is_inner(rect))
                return nullptr;
            if(!isDivide){
                return this;
            }
            // TODO: please optimize speed
            for(auto a : children){
                auto b = a->findBoundingBox(rect);
                if(b)
                    return b;
            }
            return this;
        }
        QuadTreeNode* findValuedBoundingBox(const Rect& rect){
            if(!bound.is_inner(rect))
                return nullptr;
            if(!isDivide){
                if(refs.size() == 0)
                    return nullptr;
                return this;
            }
            // TODO: please optimize speed
            for(auto a : children){
                auto b = a->findBoundingBox(rect);
                if(b)
                    return b;
            }
            return this;
        }
        void remove(int index){
            if(!isDivide){
                auto iter = std::find(refs.begin(), refs.end(),index);
                if(iter == refs.end())
                    return;
                refs.erase(iter);
                return;
            }
            auto& a = objects[index];
            auto pos = a.second;
            if(pos.y <= bound.y + bound.height/2){
                if(pos.x <= bound.x + bound.width/2)
                    children[0]->remove(index);
                else
                    children[1]->remove(index);
            }else{
                if(pos.x <= bound.x + bound.width/2)
                    children[2]->remove(index);
                else
                    children[3]->remove(index);
            }
        }
    };
    QuadTreeNode* root = nullptr;
    int totalDepth = 1;
    QuadTreeQuery(){
        root = new QuadTreeNode(this->objects);
    }
    ~QuadTreeQuery()override{
        delete root;
    }
    void divide(){
        root->divide();
        ++totalDepth;
    }

    void clear() override{
        delete root;
        objects.clear();
        root = new QuadTreeNode(this->objects);
        root->bound = bound;
    }
    void set_bound(float x,float y,float height,float width) override{
        bound.x = x;
        bound.y = y;
        bound.height = height;
        bound.width = width;
        root->bound = bound;
    }
    godot::Array get_all_points() override{
        godot::Array arr;
        for(auto& a : objects){
            arr.push_back(a.first);
        }
        return arr;
    }
    godot::PackedVector2Array get_all_positions() override{
        godot::PackedVector2Array arr;
        for(auto& a : objects){
            arr.push_back(a.second);
        }
        return arr;
    }
    void add_point(godot::Variant value, godot::Vector2 pos) override{
        objects.emplace_back(std::move(value), pos);

        if(!bound.is_inner(pos.x,pos.y))
            return;
        {
            int times = 0,temp = size;
            while(temp >>= 2){
                ++times;
            }
            if(times > totalDepth)
                divide();
        }
        root->addRef(size);
        ++size;
    }
    void update_point_pos(int index,godot::Vector2 pos) override{
        objects[index].second = pos;
    }
    void update_point_value(int index,godot::Variant value) override{
        objects[index].first = value;
    }
    godot::Variant get_point_at(int index) override{
        return objects[index].first;
    }
    godot::Vector2 get_position_at(int index) override{
        return objects[index].second;
    }
    void remove_at(int index) override{
        root->remove(index);
        objects.erase(objects.begin() + index);
        --size;
    }
    void remove_range(godot::Array indices)override{
        auto size = indices.size();
        for(long i = 0;i!= size;++i){
            remove_at(indices[i]);
        }
    }
    bool has_neighbor(godot::Vector2 pos, float radius) override{
        Rect r{pos.x - radius,pos.y - radius,radius*2,radius*2};
        auto box = root->findBoundingBox(r);
        std::function<bool(QuadTreeNode*)> f = [this,&pos,radius, &f](QuadTreeNode* box) -> bool{
            if(box->isDivide){
                for(auto a : box->children){
                    if(f(a))
                        return 1;
                }
                return 0;
            }
            for(auto i : box->refs){
                auto& pos2 = objects[i].second;
                if(pos.distance_to(pos2) <= radius)
                    return 1;
            }
            return 0;
        };

        return f(box);
    }
    godot::PackedInt32Array find_neighbors(godot::Vector2 pos, float radius, int max_count = -1) override{
        Rect r{pos.x - radius,pos.y - radius,radius*2,radius*2};
        auto box = root->findBoundingBox(r);
        godot::PackedInt32Array result;
        if (box == nullptr) return result;
        int nowSize = 0;
        std::function<void(QuadTreeNode*)> f = [this,&pos,radius,max_count, &f,&result,&nowSize](QuadTreeNode* box) -> void{
            if(box->isDivide){
                for(auto a : box->children){
                    f(a);
                }
            }
            for(auto i : box->refs){
                auto& pos2 = objects[i].second;
                if(pos.distance_to(pos2) <= radius&&nowSize <= max_count){
                    ++nowSize;
                    result.push_back(i);
                }
            }
        };

        f(box);
        return result;
    }
    godot::Array get_all_rects()
    {
        godot::Array arr;
        TraverseAndCollectBounds(*root, arr);
        return arr;
    }
    void TraverseAndCollectBounds(const QuadTreeNode& node, godot::Array& bounds) {
        bounds.push_back(godot::Rect2(node.bound.x, node.bound.y, node.bound.width, node.bound.height));
        for (const auto& child : node.children) {
            if (child == nullptr) continue;
            TraverseAndCollectBounds(*child, bounds);
        }
    }
    godot::Array find_neighbor_points(godot::Vector2 pos,float radius,int max_count = -1)override{
        auto indices = find_neighbors(pos,radius,max_count);
        godot::Array arr;
        for(auto a : indices){
            arr.push_back(objects[a].first);
        }
        return arr;
    }
    godot::Variant get_nearest_point(godot::Vector2 pos) override{
        auto a = root->findValuedBoundingBox({pos.x,pos.y,0,0});
        float distant = FLT_MAX;
        int index;
        for(auto i : a->refs){
            auto& p = objects[i].second;
            float dis = pos.distance_to(p);
            if(dis < distant){
                distant = dis;
                index = i;
            }
        }
        return objects[index].first;
    }
    int get_nearest_index(godot::Vector2 pos) override{
        auto a = root->findValuedBoundingBox({pos.x,pos.y,0,0});
        float distant = FLT_MAX;
        int index;
        for(auto i : a->refs){
            auto& p = objects[i].second;
            float dis = pos.distance_to(p);
            if(dis < distant){
                distant = dis;
                index = i;
            }
        }
        return index;
    }
    void execute(godot::Callable mapper)override{
        auto size = objects.size();
        for(int i = 0;i!=size;++i){
            mapper.call(i,objects[i].second,objects[i].first);
        }
    }
    void custom_sort(godot::Callable sorter)override{
        std::sort(objects.begin(),objects.end(),[sorter](auto a1,auto a2) -> bool{
            return sorter.call(a1.first,a2.first);
        });
    }
    void optimize()override{}
};
class HashQuery : public SpatialQuery{

};
class KDTreeQuery : public SpatialQuery{

};


#endif //SPACE_PARTITION_CLASSES_H
