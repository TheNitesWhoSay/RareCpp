#ifndef REFLECTTEST_H
#define REFLECTTEST_H
#include <gtest/gtest.h>
#include "../CppRandomAccessReflectionLib/Reflect.h"
#include <regex>
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <functional>
using namespace Reflect;

namespace FieldAggregations
{

    struct Obj {
        int first;
        float second;

        bool operator ==(const Obj & other) const { return first == other.first && second == other.second; }
    };

    struct ObjHasher {
        std::size_t operator()(const Obj & obj) const {
            return ((std::hash<int>()(obj.first) ^ (std::hash<float>()(obj.second) << 1)) >> 1);
        }
    };

    struct ObjPointerHasher {
        std::size_t operator()(const Obj* obj) const {
            return obj == nullptr ? 0 : ObjHasher()(*obj);
        }
    };

    template <typename T>
    using PField = Field<T, 0>;

    template <typename T>
    using OField = Field<T, 0, Reflected>;

    /**
        Fields handled in distinct manners...
        - Primitive
        - PrimitivePointer
        - Object
        - ObjectPointer
        - Iterable Containing (I...)
            - Primitive (IP_)
            - PrimitivePointer (IPP_)
            - PrimitivePair (IPA_) [Arrays, Adaptors, and unordered sets/multisets disallowed]
            - PrimitivePointerPair (IPPA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
            - Object (IO_)
            - ObjectPointer (IOP_)
            - ObjectPair (IOA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
            - ObjectPointerPair (IOPA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
        - PointerToIterable Containing (P...)
            - Primitive (PP_)
            - PrimitivePointer (PPP_)
            - PrimitivePair (PPA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
            - PrimitivePointerPair (PPPA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
            - Object (PO_)
            - ObjectPointer (POP_)
            - ObjectPair (POA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
            - ObjectPointerPair (POPA_) [Arrays and Adaptors, and unordered sets/multisets disallowed]
    */

    using PrimitiveField = PField<int>;
    using PrimitivePointerField = PField<int*>;

    using ObjectField = OField<Obj>;
    using ObjectPointerField = OField<Obj*>;

    // Iterable containing primitives (IP_)
    using IP_Array = PField<int[2]>;
    using IP_StlArray = PField<std::array<int,2>>;
    using IP_Vector = PField<std::vector<int>>;
    using IP_Deque = PField<std::deque<int>>;
    using IP_ForwardList = PField<std::forward_list<int>>;
    using IP_List = PField<std::list<int>>;
    using IP_Stack = PField<std::stack<int>>;
    using IP_Queue = PField<std::queue<int>>;
    using IP_PriorityQueue = PField<std::priority_queue<int>>;
    using IP_Set = PField<std::set<int>>;
    using IP_MultiSet = PField<std::multiset<int>>;
    using IP_UnorderedSet = PField<std::unordered_set<int>>;
    using IP_UnorderedMultiSet = PField<std::unordered_multiset<int>>;
    
    // Iterable containing primitive pointers (IPP_)
    using IPP_Array = PField<int*[2]>;
    using IPP_StlArray = PField<std::array<int*,2>>;
    using IPP_Vector = PField<std::vector<int*>>;
    using IPP_Deque = PField<std::deque<int*>>;
    using IPP_ForwardList = PField<std::forward_list<int*>>;
    using IPP_List = PField<std::list<int*>>;
    using IPP_Stack = PField<std::stack<int*>>;
    using IPP_Queue = PField<std::queue<int*>>;
    using IPP_PriorityQueue = PField<std::priority_queue<int*>>;
    using IPP_Set = PField<std::set<int*>>;
    using IPP_MultiSet = PField<std::multiset<int*>>;
    using IPP_UnorderedSet = PField<std::unordered_set<int*>>;
    using IPP_UnorderedMultiSet = PField<std::unordered_multiset<int*>>;
    
    // Iterable containing primitive pairs (IPA_)
    using IPA_Array = PField<std::pair<int,int>[2]>;
    using IPA_StlArray = PField<std::array<std::pair<int,int>, 2>>;
    using IPA_Vector = PField<std::vector<std::pair<int,int>>>;
    using IPA_Deque = PField<std::deque<std::pair<int,int>>>;
    using IPA_ForwardList = PField<std::forward_list<std::pair<int,int>>>;
    using IPA_List = PField<std::list<std::pair<int,int>>>;
    using IPA_Set = PField<std::set<std::pair<int,int>>>;
    using IPA_MultiSet = PField<std::multiset<std::pair<int,int>>>;
    using IPA_Map = PField<std::map<short,int>>;
    using IPA_MultiMap = PField<std::multimap<short,int>>;
    using IPA_UnorderedMap = PField<std::unordered_map<short,int>>;
    using IPA_UnorderedMultimap = PField<std::unordered_multimap<short,int>>;

    // Iterable containing primitive pointer pairs (IPPA_)
    using IPPA_Array = PField<std::pair<int,int*>[2]>;
    using IPPA_StlArray = PField<std::array<std::pair<int,int*>, 2>>;
    using IPPA_Vector = PField<std::vector<std::pair<int,int*>>>;
    using IPPA_Deque = PField<std::deque<std::pair<int,int*>>>;
    using IPPA_ForwardList = PField<std::forward_list<std::pair<int,int*>>>;
    using IPPA_List = PField<std::list<std::pair<int,int*>>>;
    using IPPA_Set = PField<std::set<std::pair<int,int*>>>;
    using IPPA_MultiSet = PField<std::multiset<std::pair<int,int*>>>;
    using IPPA_Map = PField<std::map<short,int*>>;
    using IPPA_MultiMap = PField<std::multimap<short,int*>>;
    using IPPA_UnorderedMap = PField<std::unordered_map<short,int*>>;
    using IPPA_UnorderedMultimap = PField<std::unordered_multimap<short,int*>>;

    // Iterable containing objects (IO_)
    using IO_Array = OField<Obj[2]>;
    using IO_StlArray = OField<std::array<Obj,2>>;
    using IO_Vector = OField<std::vector<Obj>>;
    using IO_Deque = OField<std::deque<Obj>>;
    using IO_ForwardList = OField<std::forward_list<Obj>>;
    using IO_List = OField<std::list<Obj>>;
    using IO_Stack = OField<std::stack<Obj>>;
    using IO_Queue = OField<std::queue<Obj>>;
    using IO_PriorityQueue = OField<std::priority_queue<Obj>>;
    using IO_Set = OField<std::set<Obj>>;
    using IO_MultiSet = OField<std::multiset<Obj>>;
    using IO_UnorderedSet = OField<std::unordered_set<Obj, ObjHasher>>;
    using IO_UnorderedMultiSet = OField<std::unordered_multiset<Obj, ObjHasher>>;
    
    // Iterable containing object pointers (IOP_)
    using IOP_Array = OField<Obj*[2]>;
    using IOP_StlArray = OField<std::array<Obj*,2>>;
    using IOP_Vector = OField<std::vector<Obj*>>;
    using IOP_Deque = OField<std::deque<Obj*>>;
    using IOP_ForwardList = OField<std::forward_list<Obj*>>;
    using IOP_List = OField<std::list<Obj*>>;
    using IOP_Stack = OField<std::stack<Obj*>>;
    using IOP_Queue = OField<std::queue<Obj*>>;
    using IOP_PriorityQueue = OField<std::priority_queue<Obj*>>;
    using IOP_Set = OField<std::set<Obj*>>;
    using IOP_MultiSet = OField<std::multiset<Obj*>>;
    using IOP_UnorderedSet = OField<std::unordered_set<Obj*, ObjPointerHasher>>;
    using IOP_UnorderedMultiSet = OField<std::unordered_multiset<Obj*, ObjPointerHasher>>;
    
    // Iterable containing object pairs (IOA_)
    using IOA_Array = OField<std::pair<int,Obj>[2]>;
    using IOA_StlArray = OField<std::array<std::pair<int,Obj>, 2>>;
    using IOA_Vector = OField<std::vector<std::pair<int,Obj>>>;
    using IOA_Deque = OField<std::deque<std::pair<int,Obj>>>;
    using IOA_ForwardList = OField<std::forward_list<std::pair<int,Obj>>>;
    using IOA_List = OField<std::list<std::pair<int,Obj>>>;
    using IOA_Set = OField<std::set<std::pair<int,Obj>>>;
    using IOA_MultiSet = OField<std::multiset<std::pair<int,Obj>>>;
    using IOA_Map = OField<std::map<short,Obj>>;
    using IOA_MultiMap = OField<std::multimap<short,Obj>>;
    using IOA_UnorderedMap = OField<std::unordered_map<short,Obj>>;
    using IOA_UnorderedMultimap = OField<std::unordered_multimap<short,Obj>>;

    // Iterable containing object pointer pairs (IOPA_)
    using IOPA_Array = OField<std::pair<int,Obj*>[2]>;
    using IOPA_StlArray = OField<std::array<std::pair<int,Obj*>, 2>>;
    using IOPA_Vector = OField<std::vector<std::pair<int,Obj*>>>;
    using IOPA_Deque = OField<std::deque<std::pair<int,Obj*>>>;
    using IOPA_ForwardList = OField<std::forward_list<std::pair<int,Obj*>>>;
    using IOPA_List = OField<std::list<std::pair<int,Obj*>>>;
    using IOPA_Set = OField<std::set<std::pair<int,Obj*>>>;
    using IOPA_MultiSet = OField<std::multiset<std::pair<int,Obj*>>>;
    using IOPA_Map = OField<std::map<short,Obj*>>;
    using IOPA_MultiMap = OField<std::multimap<short,Obj*>>;
    using IOPA_UnorderedMap = OField<std::unordered_map<short,Obj*>>;
    using IOPA_UnorderedMultimap = OField<std::unordered_multimap<short,Obj*>>;

    // Begin Pointers...
    // Pointer to iterable containing primitives (PP_)
    using PP_Array = PField<IP_Array::Type*>;
    using PP_StlArray = PField<std::array<int,2>*>;
    using PP_Vector = PField<std::vector<int>*>;
    using PP_Deque = PField<std::deque<int>*>;
    using PP_ForwardList = PField<std::forward_list<int>*>;
    using PP_List = PField<std::list<int>*>;
    using PP_Stack = PField<std::stack<int>*>;
    using PP_Queue = PField<std::queue<int>*>;
    using PP_PriorityQueue = PField<std::priority_queue<int>*>;
    using PP_Set = PField<std::set<int>*>;
    using PP_MultiSet = PField<std::multiset<int>*>;
    using PP_UnorderedSet = PField<std::unordered_set<int>*>;
    using PP_UnorderedMultiSet = PField<std::unordered_multiset<int>*>;
    
    // Pointer to iterable containing primitive pointers (PPP_)
    using PPP_Array = PField<IPP_Array::Type*>;
    using PPP_StlArray = PField<std::array<int*,2>*>;
    using PPP_Vector = PField<std::vector<int*>*>;
    using PPP_Deque = PField<std::deque<int*>*>;
    using PPP_ForwardList = PField<std::forward_list<int*>*>;
    using PPP_List = PField<std::list<int*>*>;
    using PPP_Stack = PField<std::stack<int*>*>;
    using PPP_Queue = PField<std::queue<int*>*>;
    using PPP_PriorityQueue = PField<std::priority_queue<int*>*>;
    using PPP_Set = PField<std::set<int*>*>;
    using PPP_MultiSet = PField<std::multiset<int*>*>;
    using PPP_UnorderedSet = PField<std::unordered_set<int*>*>;
    using PPP_UnorderedMultiSet = PField<std::unordered_multiset<int*>*>;
    
    // Pointer to iterable containing primitive pairs (PPA_)
    using PPA_Array = PField<IPA_Array::Type*>;
    using PPA_StlArray = PField<std::array<std::pair<int,int>, 2>*>;
    using PPA_Vector = PField<std::vector<std::pair<int,int>>*>;
    using PPA_Deque = PField<std::deque<std::pair<int,int>>*>;
    using PPA_ForwardList = PField<std::forward_list<std::pair<int,int>>*>;
    using PPA_List = PField<std::list<std::pair<int,int>>*>;
    using PPA_Set = PField<std::set<std::pair<int,int>>*>;
    using PPA_MultiSet = PField<std::multiset<std::pair<int,int>>*>;
    using PPA_Map = PField<std::map<short,int>*>;
    using PPA_MultiMap = PField<std::multimap<short,int>*>;
    using PPA_UnorderedMap = PField<std::unordered_map<short,int>*>;
    using PPA_UnorderedMultimap = PField<std::unordered_multimap<short,int>*>;

    // Pointer to iterable containing primitive pointer pairs (PPPA_)
    using PPPA_Array = PField<IPPA_Array::Type*>;
    using PPPA_StlArray = PField<std::array<std::pair<int,int*>, 2>*>;
    using PPPA_Vector = PField<std::vector<std::pair<int,int*>>*>;
    using PPPA_Deque = PField<std::deque<std::pair<int,int*>>*>;
    using PPPA_ForwardList = PField<std::forward_list<std::pair<int,int*>>*>;
    using PPPA_List = PField<std::list<std::pair<int,int*>>*>;
    using PPPA_Set = PField<std::set<std::pair<int,int*>>*>;
    using PPPA_MultiSet = PField<std::multiset<std::pair<int,int*>>*>;
    using PPPA_Map = PField<std::map<short,int*>*>;
    using PPPA_MultiMap = PField<std::multimap<short,int*>*>;
    using PPPA_UnorderedMap = PField<std::unordered_map<short,int*>*>;
    using PPPA_UnorderedMultimap = PField<std::unordered_multimap<short,int*>*>;

    // Pointer to iterable containing objects (PO_)
    using PO_Array = OField<IO_Array::Type*>;
    using PO_StlArray = OField<std::array<Obj,2>*>;
    using PO_Vector = OField<std::vector<Obj>*>;
    using PO_Deque = OField<std::deque<Obj>*>;
    using PO_ForwardList = OField<std::forward_list<Obj>*>;
    using PO_List = OField<std::list<Obj>*>;
    using PO_Stack = OField<std::stack<Obj>*>;
    using PO_Queue = OField<std::queue<Obj>*>;
    using PO_PriorityQueue = OField<std::priority_queue<Obj>*>;
    using PO_Set = OField<std::set<Obj>*>;
    using PO_MultiSet = OField<std::multiset<Obj>*>;
    using PO_UnorderedSet = OField<std::unordered_set<Obj, ObjHasher>*>;
    using PO_UnorderedMultiSet = OField<std::unordered_multiset<Obj, ObjHasher>*>;
    
    // Pointer to iterable containing object pointers (POP_)
    using POP_Array = OField<IOP_Array::Type*>;
    using POP_StlArray = OField<std::array<Obj*,2>*>;
    using POP_Vector = OField<std::vector<Obj*>*>;
    using POP_Deque = OField<std::deque<Obj*>*>;
    using POP_ForwardList = OField<std::forward_list<Obj*>*>;
    using POP_List = OField<std::list<Obj*>*>;
    using POP_Stack = OField<std::stack<Obj*>*>;
    using POP_Queue = OField<std::queue<Obj*>*>;
    using POP_PriorityQueue = OField<std::priority_queue<Obj*>*>;
    using POP_Set = OField<std::set<Obj*>*>;
    using POP_MultiSet = OField<std::multiset<Obj*>*>;
    using POP_UnorderedSet = OField<std::unordered_set<Obj*, ObjPointerHasher>*>;
    using POP_UnorderedMultiSet = OField<std::unordered_multiset<Obj*, ObjPointerHasher>*>;
    
    // Pointer to iterable containing object pairs (POA_)
    using POA_Array = OField<IOA_Array::Type*>;
    using POA_StlArray = OField<std::array<std::pair<int,Obj>, 2>*>;
    using POA_Vector = OField<std::vector<std::pair<int,Obj>>*>;
    using POA_Deque = OField<std::deque<std::pair<int,Obj>>*>;
    using POA_ForwardList = OField<std::forward_list<std::pair<int,Obj>>*>;
    using POA_List = OField<std::list<std::pair<int,Obj>>*>;
    using POA_Set = OField<std::set<std::pair<int,Obj>>*>;
    using POA_MultiSet = OField<std::multiset<std::pair<int,Obj>>*>;
    using POA_Map = OField<std::map<short,Obj>*>;
    using POA_MultiMap = OField<std::multimap<short,Obj>*>;
    using POA_UnorderedMap = OField<std::unordered_map<short,Obj>*>;
    using POA_UnorderedMultimap = OField<std::unordered_multimap<short,Obj>*>;

    // Pointer to iterable containing object pointer pairs (POPA_)
    using POPA_Array = OField<IOPA_Array::Type*>;
    using POPA_StlArray = OField<std::array<std::pair<int,Obj*>, 2>*>;
    using POPA_Vector = OField<std::vector<std::pair<int,Obj*>>*>;
    using POPA_Deque = OField<std::deque<std::pair<int,Obj*>>*>;
    using POPA_ForwardList = OField<std::forward_list<std::pair<int,Obj*>>*>;
    using POPA_List = OField<std::list<std::pair<int,Obj*>>*>;
    using POPA_Set = OField<std::set<std::pair<int,Obj*>>*>;
    using POPA_MultiSet = OField<std::multiset<std::pair<int,Obj*>>*>;
    using POPA_Map = OField<std::map<short,Obj*>*>;
    using POPA_MultiMap = OField<std::multimap<short,Obj*>*>;
    using POPA_UnorderedMap = OField<std::unordered_map<short,Obj*>*>;
    using POPA_UnorderedMultimap = OField<std::unordered_multimap<short,Obj*>*>;
    
    extern PrimitiveField primitive;
    extern PrimitivePointerField primitivePointer;

    extern ObjectField object;
    extern ObjectPointerField objectPointer;

    extern IP_Array ipArray;
    extern IP_StlArray ipStlArray;
    extern IP_Vector ipVector;
    extern IP_Deque ipDeque;
    extern IP_ForwardList ipForwardList;
    extern IP_List ipList;
    extern IP_Stack ipStack;
    extern IP_Queue ipQueue;
    extern IP_PriorityQueue ipPriorityQueue;
    extern IP_Set ipSet;
    extern IP_MultiSet ipMultiSet;
    extern IP_UnorderedSet ipUnorderedSet;
    extern IP_UnorderedMultiSet ipUnorderedMultiSet;

    extern IPP_Array ippArray;
    extern IPP_StlArray ippStlArray;
    extern IPP_Vector ippVector;
    extern IPP_Deque ippDeque;
    extern IPP_ForwardList ippForwardList;
    extern IPP_List ippList;
    extern IPP_Stack ippStack;
    extern IPP_Queue ippQueue;
    extern IPP_PriorityQueue ippPriorityQueue;
    extern IPP_Set ippSet;
    extern IPP_MultiSet ippMultiSet;
    extern IPP_UnorderedSet ippUnorderedSet;
    extern IPP_UnorderedMultiSet ippUnorderedMultiSet;

    extern IPA_Array ipaArray;
    extern IPA_StlArray ipaStlArray;
    extern IPA_Vector ipaVector;
    extern IPA_Deque ipaDeque;
    extern IPA_ForwardList ipaForwardList;
    extern IPA_List ipaList;
    extern IPA_Set ipaSet;
    extern IPA_MultiSet ipaMultiSet;
    extern IPA_Map ipaMap;
    extern IPA_MultiMap ipaMultiMap;
    extern IPA_UnorderedMap ipaUnorderedMap;
    extern IPA_UnorderedMultimap ipaUnorderedMultimap;

    extern IPPA_Array ippaArray;
    extern IPPA_StlArray ippaStlArray;
    extern IPPA_Vector ippaVector;
    extern IPPA_Deque ippaDeque;
    extern IPPA_ForwardList ippaForwardList;
    extern IPPA_List ippaList;
    extern IPPA_Set ippaSet;
    extern IPPA_MultiSet ippaMultiSet;
    extern IPPA_Map ippaMap;
    extern IPPA_MultiMap ippaMultiMap;
    extern IPPA_UnorderedMap ippaUnorderedMap;
    extern IPPA_UnorderedMultimap ippaUnorderedMultimap;

    extern IO_Array ioArray;
    extern IO_StlArray ioStlArray;
    extern IO_Vector ioVector;
    extern IO_Deque ioDeque;
    extern IO_ForwardList ioForwardList;
    extern IO_List ioList;
    extern IO_Stack ioStack;
    extern IO_Queue ioQueue;
    extern IO_PriorityQueue ioPriorityQueue;
    extern IO_Set ioSet;
    extern IO_MultiSet ioMultiSet;
    extern IO_UnorderedSet ioUnorderedSet;
    extern IO_UnorderedMultiSet ioUnorderedMultiSet;

    extern IOP_Array iopArray;
    extern IOP_StlArray iopStlArray;
    extern IOP_Vector iopVector;
    extern IOP_Deque iopDeque;
    extern IOP_ForwardList iopForwardList;
    extern IOP_List iopList;
    extern IOP_Stack iopStack;
    extern IOP_Queue iopQueue;
    extern IOP_PriorityQueue iopPriorityQueue;
    extern IOP_Set iopSet;
    extern IOP_MultiSet iopMultiSet;
    extern IOP_UnorderedSet iopUnorderedSet;
    extern IOP_UnorderedMultiSet iopUnorderedMultiSet;

    extern IOA_Array ioaArray;
    extern IOA_StlArray ioaStlArray;
    extern IOA_Vector ioaVector;
    extern IOA_Deque ioaDeque;
    extern IOA_ForwardList ioaForwardList;
    extern IOA_List ioaList;
    extern IOA_Set ioaSet;
    extern IOA_MultiSet ioaMultiSet;
    extern IOA_Map ioaMap;
    extern IOA_MultiMap ioaMultiMap;
    extern IOA_UnorderedMap ioaUnorderedMap;
    extern IOA_UnorderedMultimap ioaUnorderedMultimap;

    extern IOPA_Array iopaArray;
    extern IOPA_StlArray iopaStlArray;
    extern IOPA_Vector iopaVector;
    extern IOPA_Deque iopaDeque;
    extern IOPA_ForwardList iopaForwardList;
    extern IOPA_List iopaList;
    extern IOPA_Set iopaSet;
    extern IOPA_MultiSet iopaMultiSet;
    extern IOPA_Map iopaMap;
    extern IOPA_MultiMap iopaMultiMap;
    extern IOPA_UnorderedMap iopaUnorderedMap;
    extern IOPA_UnorderedMultimap iopaUnorderedMultimap;



    extern PP_Array ppArray;
    extern PP_StlArray ppStlArray;
    extern PP_Vector ppVector;
    extern PP_Deque ppDeque;
    extern PP_ForwardList ppForwardList;
    extern PP_List ppList;
    extern PP_Stack ppStack;
    extern PP_Queue ppQueue;
    extern PP_PriorityQueue ppPriorityQueue;
    extern PP_Set ppSet;
    extern PP_MultiSet ppMultiSet;
    extern PP_UnorderedSet ppUnorderedSet;
    extern PP_UnorderedMultiSet ppUnorderedMultiSet;

    extern PPP_Array pppArray;
    extern PPP_StlArray pppStlArray;
    extern PPP_Vector pppVector;
    extern PPP_Deque pppDeque;
    extern PPP_ForwardList pppForwardList;
    extern PPP_List pppList;
    extern PPP_Stack pppStack;
    extern PPP_Queue pppQueue;
    extern PPP_PriorityQueue pppPriorityQueue;
    extern PPP_Set pppSet;
    extern PPP_MultiSet pppMultiSet;
    extern PPP_UnorderedSet pppUnorderedSet;
    extern PPP_UnorderedMultiSet pppUnorderedMultiSet;

    extern PPA_Array ppaArray;
    extern PPA_StlArray ppaStlArray;
    extern PPA_Vector ppaVector;
    extern PPA_Deque ppaDeque;
    extern PPA_ForwardList ppaForwardList;
    extern PPA_List ppaList;
    extern PPA_Set ppaSet;
    extern PPA_MultiSet ppaMultiSet;
    extern PPA_Map ppaMap;
    extern PPA_MultiMap ppaMultiMap;
    extern PPA_UnorderedMap ppaUnorderedMap;
    extern PPA_UnorderedMultimap ppaUnorderedMultimap;

    extern PPPA_Array pppaArray;
    extern PPPA_StlArray pppaStlArray;
    extern PPPA_Vector pppaVector;
    extern PPPA_Deque pppaDeque;
    extern PPPA_ForwardList pppaForwardList;
    extern PPPA_List pppaList;
    extern PPPA_Set pppaSet;
    extern PPPA_MultiSet pppaMultiSet;
    extern PPPA_Map pppaMap;
    extern PPPA_MultiMap pppaMultiMap;
    extern PPPA_UnorderedMap pppaUnorderedMap;
    extern PPPA_UnorderedMultimap pppaUnorderedMultimap;

    extern PO_Array poArray;
    extern PO_StlArray poStlArray;
    extern PO_Vector poVector;
    extern PO_Deque poDeque;
    extern PO_ForwardList poForwardList;
    extern PO_List poList;
    extern PO_Stack poStack;
    extern PO_Queue poQueue;
    extern PO_PriorityQueue poPriorityQueue;
    extern PO_Set poSet;
    extern PO_MultiSet poMultiSet;
    extern PO_UnorderedSet poUnorderedSet;
    extern PO_UnorderedMultiSet poUnorderedMultiSet;

    extern POP_Array popArray;
    extern POP_StlArray popStlArray;
    extern POP_Vector popVector;
    extern POP_Deque popDeque;
    extern POP_ForwardList popForwardList;
    extern POP_List popList;
    extern POP_Stack popStack;
    extern POP_Queue popQueue;
    extern POP_PriorityQueue popPriorityQueue;
    extern POP_Set popSet;
    extern POP_MultiSet popMultiSet;
    extern POP_UnorderedSet popUnorderedSet;
    extern POP_UnorderedMultiSet popUnorderedMultiSet;

    extern POA_Array poaArray;
    extern POA_StlArray poaStlArray;
    extern POA_Vector poaVector;
    extern POA_Deque poaDeque;
    extern POA_ForwardList poaForwardList;
    extern POA_List poaList;
    extern POA_Set poaSet;
    extern POA_MultiSet poaMultiSet;
    extern POA_Map poaMap;
    extern POA_MultiMap poaMultiMap;
    extern POA_UnorderedMap poaUnorderedMap;
    extern POA_UnorderedMultimap poaUnorderedMultimap;

    extern POPA_Array popaArray;
    extern POPA_StlArray popaStlArray;
    extern POPA_Vector popaVector;
    extern POPA_Deque popaDeque;
    extern POPA_ForwardList popaForwardList;
    extern POPA_List popaList;
    extern POPA_Set popaSet;
    extern POPA_MultiSet popaMultiSet;
    extern POPA_Map popaMap;
    extern POPA_MultiMap popaMultiMap;
    extern POPA_UnorderedMap popaUnorderedMap;
    extern POPA_UnorderedMultimap popaUnorderedMultimap;


    extern decltype(primitive)::Type primitiveVal;

    extern decltype(primitivePointer)::Type primitivePointerVal;

    extern decltype(object)::Type objectVal;

    extern decltype(objectPointer)::Type objectPointerVal;


    extern decltype(ipArray)::Type ipArrayVal;
    extern decltype(ipStlArray)::Type ipStlArrayVal;
    extern decltype(ipVector)::Type ipVectorVal;
    extern decltype(ipDeque)::Type ipDequeVal;
    extern decltype(ipForwardList)::Type ipForwardListVal;
    extern decltype(ipList)::Type ipListVal;
    extern decltype(ipStack)::Type ipStackVal;
    extern decltype(ipQueue)::Type ipQueueVal;
    extern decltype(ipPriorityQueue)::Type ipPriorityQueueVal;
    extern decltype(ipSet)::Type ipSetVal;
    extern decltype(ipMultiSet)::Type ipMultiSetVal;
    extern decltype(ipUnorderedSet)::Type ipUnorderedSetVal;
    extern decltype(ipUnorderedMultiSet)::Type ipUnorderedMultiSetVal;

    extern decltype(ippArray)::Type ippArrayVal;
    extern decltype(ippStlArray)::Type ippStlArrayVal;
    extern decltype(ippVector)::Type ippVectorVal;
    extern decltype(ippDeque)::Type ippDequeVal;
    extern decltype(ippForwardList)::Type ippForwardListVal;
    extern decltype(ippList)::Type ippListVal;
    extern decltype(ippStack)::Type ippStackVal;
    extern decltype(ippQueue)::Type ippQueueVal;
    extern decltype(ippPriorityQueue)::Type ippPriorityQueueVal;
    extern decltype(ippSet)::Type ippSetVal;
    extern decltype(ippMultiSet)::Type ippMultiSetVal;
    extern decltype(ippUnorderedSet)::Type ippUnorderedSetVal;
    extern decltype(ippUnorderedMultiSet)::Type ippUnorderedMultiSetVal;

    extern decltype(ipaStlArray)::Type ipaStlArrayVal;
    extern decltype(ipaVector)::Type ipaVectorVal;
    extern decltype(ipaDeque)::Type ipaDequeVal;
    extern decltype(ipaForwardList)::Type ipaForwardListVal;
    extern decltype(ipaList)::Type ipaListVal;
    extern decltype(ipaSet)::Type ipaSetVal;
    extern decltype(ipaMultiSet)::Type ipaMultiSetVal;
    extern decltype(ipaMap)::Type ipaMapVal;
    extern decltype(ipaMultiMap)::Type ipaMultiMapVal;
    extern decltype(ipaUnorderedMap)::Type ipaUnorderedMapVal;
    extern decltype(ipaUnorderedMultimap)::Type ipaUnorderedMultimapVal;

    extern decltype(ippaStlArray)::Type ippaStlArrayVal;
    extern decltype(ippaVector)::Type ippaVectorVal;
    extern decltype(ippaDeque)::Type ippaDequeVal;
    extern decltype(ippaForwardList)::Type ippaForwardListVal;
    extern decltype(ippaList)::Type ippaListVal;
    extern decltype(ippaSet)::Type ippaSetVal;
    extern decltype(ippaMultiSet)::Type ippaMultiSetVal;
    extern decltype(ippaMap)::Type ippaMapVal;
    extern decltype(ippaMultiMap)::Type ippaMultiMapVal;
    extern decltype(ippaUnorderedMap)::Type ippaUnorderedMapVal;
    extern decltype(ippaUnorderedMultimap)::Type ippaUnorderedMultimapVal;

    extern decltype(ioArray)::Type ioArrayVal;
    extern decltype(ioStlArray)::Type ioStlArrayVal;
    extern decltype(ioVector)::Type ioVectorVal;
    extern decltype(ioDeque)::Type ioDequeVal;
    extern decltype(ioForwardList)::Type ioForwardListVal;
    extern decltype(ioList)::Type ioListVal;
    extern decltype(ioStack)::Type ioStackVal;
    extern decltype(ioQueue)::Type ioQueueVal;
    extern decltype(ioPriorityQueue)::Type ioPriorityQueueVal;
    extern decltype(ioSet)::Type ioSetVal;
    extern decltype(ioMultiSet)::Type ioMultiSetVal;
    extern decltype(ioUnorderedSet)::Type ioUnorderedSetVal;
    extern decltype(ioUnorderedMultiSet)::Type ioUnorderedMultiSetVal;

    extern decltype(iopArray)::Type iopArrayVal;
    extern decltype(iopStlArray)::Type iopStlArrayVal;
    extern decltype(iopVector)::Type iopVectorVal;
    extern decltype(iopDeque)::Type iopDequeVal;
    extern decltype(iopForwardList)::Type iopForwardListVal;
    extern decltype(iopList)::Type iopListVal;
    extern decltype(iopStack)::Type iopStackVal;
    extern decltype(iopQueue)::Type iopQueueVal;
    extern decltype(iopPriorityQueue)::Type iopPriorityQueueVal;
    extern decltype(iopSet)::Type iopSetVal;
    extern decltype(iopMultiSet)::Type iopMultiSetVal;
    extern decltype(iopUnorderedSet)::Type iopUnorderedSetVal;
    extern decltype(iopUnorderedMultiSet)::Type iopUnorderedMultiSetVal;

    extern decltype(ioaStlArray)::Type ioaStlArrayVal;
    extern decltype(ioaVector)::Type ioaVectorVal;
    extern decltype(ioaDeque)::Type ioaDequeVal;
    extern decltype(ioaForwardList)::Type ioaForwardListVal;
    extern decltype(ioaList)::Type ioaListVal;
    extern decltype(ioaSet)::Type ioaSetVal;
    extern decltype(ioaMultiSet)::Type ioaMultiSetVal;
    extern decltype(ioaMap)::Type ioaMapVal;
    extern decltype(ioaMultiMap)::Type ioaMultiMapVal;
    extern decltype(ioaUnorderedMap)::Type ioaUnorderedMapVal;
    extern decltype(ioaUnorderedMultimap)::Type ioaUnorderedMultimapVal;

    extern decltype(iopaStlArray)::Type iopaStlArrayVal;
    extern decltype(iopaVector)::Type iopaVectorVal;
    extern decltype(iopaDeque)::Type iopaDequeVal;
    extern decltype(iopaForwardList)::Type iopaForwardListVal;
    extern decltype(iopaList)::Type iopaListVal;
    extern decltype(iopaSet)::Type iopaSetVal;
    extern decltype(iopaMultiSet)::Type iopaMultiSetVal;
    extern decltype(iopaMap)::Type iopaMapVal;
    extern decltype(iopaMultiMap)::Type iopaMultiMapVal;
    extern decltype(iopaUnorderedMap)::Type iopaUnorderedMapVal;
    extern decltype(iopaUnorderedMultimap)::Type iopaUnorderedMultimapVal;

    extern decltype(ppArray)::Type ppArrayVal;
    extern decltype(ppStlArray)::Type ppStlArrayVal;
    extern decltype(ppVector)::Type ppVectorVal;
    extern decltype(ppDeque)::Type ppDequeVal;
    extern decltype(ppForwardList)::Type ppForwardListVal;
    extern decltype(ppList)::Type ppListVal;
    extern decltype(ppStack)::Type ppStackVal;
    extern decltype(ppQueue)::Type ppQueueVal;
    extern decltype(ppPriorityQueue)::Type ppPriorityQueueVal;
    extern decltype(ppSet)::Type ppSetVal;
    extern decltype(ppMultiSet)::Type ppMultiSetVal;
    extern decltype(ppUnorderedSet)::Type ppUnorderedSetVal;
    extern decltype(ppUnorderedMultiSet)::Type ppUnorderedMultiSetVal;

    extern decltype(pppArray)::Type pppArrayVal;
    extern decltype(pppStlArray)::Type pppStlArrayVal;
    extern decltype(pppVector)::Type pppVectorVal;
    extern decltype(pppDeque)::Type pppDequeVal;
    extern decltype(pppForwardList)::Type pppForwardListVal;
    extern decltype(pppList)::Type pppListVal;
    extern decltype(pppStack)::Type pppStackVal;
    extern decltype(pppQueue)::Type pppQueueVal;
    extern decltype(pppPriorityQueue)::Type pppPriorityQueueVal;
    extern decltype(pppSet)::Type pppSetVal;
    extern decltype(pppMultiSet)::Type pppMultiSetVal;
    extern decltype(pppUnorderedSet)::Type pppUnorderedSetVal;
    extern decltype(pppUnorderedMultiSet)::Type pppUnorderedMultiSetVal;

    extern decltype(ppaStlArray)::Type ppaStlArrayVal;
    extern decltype(ppaVector)::Type ppaVectorVal;
    extern decltype(ppaDeque)::Type ppaDequeVal;
    extern decltype(ppaForwardList)::Type ppaForwardListVal;
    extern decltype(ppaList)::Type ppaListVal;
    extern decltype(ppaSet)::Type ppaSetVal;
    extern decltype(ppaMultiSet)::Type ppaMultiSetVal;
    extern decltype(ppaMap)::Type ppaMapVal;
    extern decltype(ppaMultiMap)::Type ppaMultiMapVal;
    extern decltype(ppaUnorderedMap)::Type ppaUnorderedMapVal;
    extern decltype(ppaUnorderedMultimap)::Type ppaUnorderedMultimapVal;

    extern decltype(pppaStlArray)::Type pppaStlArrayVal;
    extern decltype(pppaVector)::Type pppaVectorVal;
    extern decltype(pppaDeque)::Type pppaDequeVal;
    extern decltype(pppaForwardList)::Type pppaForwardListVal;
    extern decltype(pppaList)::Type pppaListVal;
    extern decltype(pppaSet)::Type pppaSetVal;
    extern decltype(pppaMultiSet)::Type pppaMultiSetVal;
    extern decltype(pppaMap)::Type pppaMapVal;
    extern decltype(pppaMultiMap)::Type pppaMultiMapVal;
    extern decltype(pppaUnorderedMap)::Type pppaUnorderedMapVal;
    extern decltype(pppaUnorderedMultimap)::Type pppaUnorderedMultimapVal;

    extern decltype(poArray)::Type poArrayVal;
    extern decltype(poStlArray)::Type poStlArrayVal;
    extern decltype(poVector)::Type poVectorVal;
    extern decltype(poDeque)::Type poDequeVal;
    extern decltype(poForwardList)::Type poForwardListVal;
    extern decltype(poList)::Type poListVal;
    extern decltype(poStack)::Type poStackVal;
    extern decltype(poQueue)::Type poQueueVal;
    extern decltype(poPriorityQueue)::Type poPriorityQueueVal;
    extern decltype(poSet)::Type poSetVal;
    extern decltype(poMultiSet)::Type poMultiSetVal;
    extern decltype(poUnorderedSet)::Type poUnorderedSetVal;
    extern decltype(poUnorderedMultiSet)::Type poUnorderedMultiSetVal;

    extern decltype(popArray)::Type popArrayVal;
    extern decltype(popStlArray)::Type popStlArrayVal;
    extern decltype(popVector)::Type popVectorVal;
    extern decltype(popDeque)::Type popDequeVal;
    extern decltype(popForwardList)::Type popForwardListVal;
    extern decltype(popList)::Type popListVal;
    extern decltype(popStack)::Type popStackVal;
    extern decltype(popQueue)::Type popQueueVal;
    extern decltype(popPriorityQueue)::Type popPriorityQueueVal;
    extern decltype(popSet)::Type popSetVal;
    extern decltype(popMultiSet)::Type popMultiSetVal;
    extern decltype(popUnorderedSet)::Type popUnorderedSetVal;
    extern decltype(popUnorderedMultiSet)::Type popUnorderedMultiSetVal;

    extern decltype(poaStlArray)::Type poaStlArrayVal;
    extern decltype(poaVector)::Type poaVectorVal;
    extern decltype(poaDeque)::Type poaDequeVal;
    extern decltype(poaForwardList)::Type poaForwardListVal;
    extern decltype(poaList)::Type poaListVal;
    extern decltype(poaSet)::Type poaSetVal;
    extern decltype(poaMultiSet)::Type poaMultiSetVal;
    extern decltype(poaMap)::Type poaMapVal;
    extern decltype(poaMultiMap)::Type poaMultiMapVal;
    extern decltype(poaUnorderedMap)::Type poaUnorderedMapVal;
    extern decltype(poaUnorderedMultimap)::Type poaUnorderedMultimapVal;

    extern decltype(popaStlArray)::Type popaStlArrayVal;
    extern decltype(popaVector)::Type popaVectorVal;
    extern decltype(popaDeque)::Type popaDequeVal;
    extern decltype(popaForwardList)::Type popaForwardListVal;
    extern decltype(popaList)::Type popaListVal;
    extern decltype(popaSet)::Type popaSetVal;
    extern decltype(popaMultiSet)::Type popaMultiSetVal;
    extern decltype(popaMap)::Type popaMapVal;
    extern decltype(popaMultiMap)::Type popaMultiMapVal;
    extern decltype(popaUnorderedMap)::Type popaUnorderedMapVal;
    extern decltype(popaUnorderedMultimap)::Type popaUnorderedMultimapVal;

    extern void* validPointer;

    constexpr bool T = true; // true
    constexpr bool F = false; // false
    extern bool v; // visited
    extern std::function<void(int)> f;

}

#endif