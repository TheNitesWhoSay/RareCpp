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

namespace FieldLambdaGuards
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
    using PField = RfS::Field<T, false>;

    template <typename T>
    using OField = RfS::Field<T, true>;

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
    using PP_Array = PField<IP_Array::type*>;
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
    using PPP_Array = PField<IPP_Array::type*>;
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
    using PPA_Array = PField<IPA_Array::type*>;
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
    using PPPA_Array = PField<IPPA_Array::type*>;
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
    using PO_Array = OField<IO_Array::type*>;
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
    using POP_Array = OField<IOP_Array::type*>;
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
    using POA_Array = OField<IOA_Array::type*>;
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
    using POPA_Array = OField<IOPA_Array::type*>;
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


    extern decltype(primitive)::type primitiveVal;

    extern decltype(primitivePointer)::type primitivePointerVal;

    extern decltype(object)::type objectVal;

    extern decltype(objectPointer)::type objectPointerVal;


    extern decltype(ipArray)::type ipArrayVal;
    extern decltype(ipStlArray)::type ipStlArrayVal;
    extern decltype(ipVector)::type ipVectorVal;
    extern decltype(ipDeque)::type ipDequeVal;
    extern decltype(ipForwardList)::type ipForwardListVal;
    extern decltype(ipList)::type ipListVal;
    extern decltype(ipStack)::type ipStackVal;
    extern decltype(ipQueue)::type ipQueueVal;
    extern decltype(ipPriorityQueue)::type ipPriorityQueueVal;
    extern decltype(ipSet)::type ipSetVal;
    extern decltype(ipMultiSet)::type ipMultiSetVal;
    extern decltype(ipUnorderedSet)::type ipUnorderedSetVal;
    extern decltype(ipUnorderedMultiSet)::type ipUnorderedMultiSetVal;

    extern decltype(ippArray)::type ippArrayVal;
    extern decltype(ippStlArray)::type ippStlArrayVal;
    extern decltype(ippVector)::type ippVectorVal;
    extern decltype(ippDeque)::type ippDequeVal;
    extern decltype(ippForwardList)::type ippForwardListVal;
    extern decltype(ippList)::type ippListVal;
    extern decltype(ippStack)::type ippStackVal;
    extern decltype(ippQueue)::type ippQueueVal;
    extern decltype(ippPriorityQueue)::type ippPriorityQueueVal;
    extern decltype(ippSet)::type ippSetVal;
    extern decltype(ippMultiSet)::type ippMultiSetVal;
    extern decltype(ippUnorderedSet)::type ippUnorderedSetVal;
    extern decltype(ippUnorderedMultiSet)::type ippUnorderedMultiSetVal;

    extern decltype(ipaStlArray)::type ipaStlArrayVal;
    extern decltype(ipaVector)::type ipaVectorVal;
    extern decltype(ipaDeque)::type ipaDequeVal;
    extern decltype(ipaForwardList)::type ipaForwardListVal;
    extern decltype(ipaList)::type ipaListVal;
    extern decltype(ipaSet)::type ipaSetVal;
    extern decltype(ipaMultiSet)::type ipaMultiSetVal;
    extern decltype(ipaMap)::type ipaMapVal;
    extern decltype(ipaMultiMap)::type ipaMultiMapVal;
    extern decltype(ipaUnorderedMap)::type ipaUnorderedMapVal;
    extern decltype(ipaUnorderedMultimap)::type ipaUnorderedMultimapVal;

    extern decltype(ippaStlArray)::type ippaStlArrayVal;
    extern decltype(ippaVector)::type ippaVectorVal;
    extern decltype(ippaDeque)::type ippaDequeVal;
    extern decltype(ippaForwardList)::type ippaForwardListVal;
    extern decltype(ippaList)::type ippaListVal;
    extern decltype(ippaSet)::type ippaSetVal;
    extern decltype(ippaMultiSet)::type ippaMultiSetVal;
    extern decltype(ippaMap)::type ippaMapVal;
    extern decltype(ippaMultiMap)::type ippaMultiMapVal;
    extern decltype(ippaUnorderedMap)::type ippaUnorderedMapVal;
    extern decltype(ippaUnorderedMultimap)::type ippaUnorderedMultimapVal;

    extern decltype(ioArray)::type ioArrayVal;
    extern decltype(ioStlArray)::type ioStlArrayVal;
    extern decltype(ioVector)::type ioVectorVal;
    extern decltype(ioDeque)::type ioDequeVal;
    extern decltype(ioForwardList)::type ioForwardListVal;
    extern decltype(ioList)::type ioListVal;
    extern decltype(ioStack)::type ioStackVal;
    extern decltype(ioQueue)::type ioQueueVal;
    extern decltype(ioPriorityQueue)::type ioPriorityQueueVal;
    extern decltype(ioSet)::type ioSetVal;
    extern decltype(ioMultiSet)::type ioMultiSetVal;
    extern decltype(ioUnorderedSet)::type ioUnorderedSetVal;
    extern decltype(ioUnorderedMultiSet)::type ioUnorderedMultiSetVal;

    extern decltype(iopArray)::type iopArrayVal;
    extern decltype(iopStlArray)::type iopStlArrayVal;
    extern decltype(iopVector)::type iopVectorVal;
    extern decltype(iopDeque)::type iopDequeVal;
    extern decltype(iopForwardList)::type iopForwardListVal;
    extern decltype(iopList)::type iopListVal;
    extern decltype(iopStack)::type iopStackVal;
    extern decltype(iopQueue)::type iopQueueVal;
    extern decltype(iopPriorityQueue)::type iopPriorityQueueVal;
    extern decltype(iopSet)::type iopSetVal;
    extern decltype(iopMultiSet)::type iopMultiSetVal;
    extern decltype(iopUnorderedSet)::type iopUnorderedSetVal;
    extern decltype(iopUnorderedMultiSet)::type iopUnorderedMultiSetVal;

    extern decltype(ioaStlArray)::type ioaStlArrayVal;
    extern decltype(ioaVector)::type ioaVectorVal;
    extern decltype(ioaDeque)::type ioaDequeVal;
    extern decltype(ioaForwardList)::type ioaForwardListVal;
    extern decltype(ioaList)::type ioaListVal;
    extern decltype(ioaSet)::type ioaSetVal;
    extern decltype(ioaMultiSet)::type ioaMultiSetVal;
    extern decltype(ioaMap)::type ioaMapVal;
    extern decltype(ioaMultiMap)::type ioaMultiMapVal;
    extern decltype(ioaUnorderedMap)::type ioaUnorderedMapVal;
    extern decltype(ioaUnorderedMultimap)::type ioaUnorderedMultimapVal;

    extern decltype(iopaStlArray)::type iopaStlArrayVal;
    extern decltype(iopaVector)::type iopaVectorVal;
    extern decltype(iopaDeque)::type iopaDequeVal;
    extern decltype(iopaForwardList)::type iopaForwardListVal;
    extern decltype(iopaList)::type iopaListVal;
    extern decltype(iopaSet)::type iopaSetVal;
    extern decltype(iopaMultiSet)::type iopaMultiSetVal;
    extern decltype(iopaMap)::type iopaMapVal;
    extern decltype(iopaMultiMap)::type iopaMultiMapVal;
    extern decltype(iopaUnorderedMap)::type iopaUnorderedMapVal;
    extern decltype(iopaUnorderedMultimap)::type iopaUnorderedMultimapVal;

    extern decltype(ppArray)::type ppArrayVal;
    extern decltype(ppStlArray)::type ppStlArrayVal;
    extern decltype(ppVector)::type ppVectorVal;
    extern decltype(ppDeque)::type ppDequeVal;
    extern decltype(ppForwardList)::type ppForwardListVal;
    extern decltype(ppList)::type ppListVal;
    extern decltype(ppStack)::type ppStackVal;
    extern decltype(ppQueue)::type ppQueueVal;
    extern decltype(ppPriorityQueue)::type ppPriorityQueueVal;
    extern decltype(ppSet)::type ppSetVal;
    extern decltype(ppMultiSet)::type ppMultiSetVal;
    extern decltype(ppUnorderedSet)::type ppUnorderedSetVal;
    extern decltype(ppUnorderedMultiSet)::type ppUnorderedMultiSetVal;

    extern decltype(pppArray)::type pppArrayVal;
    extern decltype(pppStlArray)::type pppStlArrayVal;
    extern decltype(pppVector)::type pppVectorVal;
    extern decltype(pppDeque)::type pppDequeVal;
    extern decltype(pppForwardList)::type pppForwardListVal;
    extern decltype(pppList)::type pppListVal;
    extern decltype(pppStack)::type pppStackVal;
    extern decltype(pppQueue)::type pppQueueVal;
    extern decltype(pppPriorityQueue)::type pppPriorityQueueVal;
    extern decltype(pppSet)::type pppSetVal;
    extern decltype(pppMultiSet)::type pppMultiSetVal;
    extern decltype(pppUnorderedSet)::type pppUnorderedSetVal;
    extern decltype(pppUnorderedMultiSet)::type pppUnorderedMultiSetVal;

    extern decltype(ppaStlArray)::type ppaStlArrayVal;
    extern decltype(ppaVector)::type ppaVectorVal;
    extern decltype(ppaDeque)::type ppaDequeVal;
    extern decltype(ppaForwardList)::type ppaForwardListVal;
    extern decltype(ppaList)::type ppaListVal;
    extern decltype(ppaSet)::type ppaSetVal;
    extern decltype(ppaMultiSet)::type ppaMultiSetVal;
    extern decltype(ppaMap)::type ppaMapVal;
    extern decltype(ppaMultiMap)::type ppaMultiMapVal;
    extern decltype(ppaUnorderedMap)::type ppaUnorderedMapVal;
    extern decltype(ppaUnorderedMultimap)::type ppaUnorderedMultimapVal;

    extern decltype(pppaStlArray)::type pppaStlArrayVal;
    extern decltype(pppaVector)::type pppaVectorVal;
    extern decltype(pppaDeque)::type pppaDequeVal;
    extern decltype(pppaForwardList)::type pppaForwardListVal;
    extern decltype(pppaList)::type pppaListVal;
    extern decltype(pppaSet)::type pppaSetVal;
    extern decltype(pppaMultiSet)::type pppaMultiSetVal;
    extern decltype(pppaMap)::type pppaMapVal;
    extern decltype(pppaMultiMap)::type pppaMultiMapVal;
    extern decltype(pppaUnorderedMap)::type pppaUnorderedMapVal;
    extern decltype(pppaUnorderedMultimap)::type pppaUnorderedMultimapVal;

    extern decltype(poArray)::type poArrayVal;
    extern decltype(poStlArray)::type poStlArrayVal;
    extern decltype(poVector)::type poVectorVal;
    extern decltype(poDeque)::type poDequeVal;
    extern decltype(poForwardList)::type poForwardListVal;
    extern decltype(poList)::type poListVal;
    extern decltype(poStack)::type poStackVal;
    extern decltype(poQueue)::type poQueueVal;
    extern decltype(poPriorityQueue)::type poPriorityQueueVal;
    extern decltype(poSet)::type poSetVal;
    extern decltype(poMultiSet)::type poMultiSetVal;
    extern decltype(poUnorderedSet)::type poUnorderedSetVal;
    extern decltype(poUnorderedMultiSet)::type poUnorderedMultiSetVal;

    extern decltype(popArray)::type popArrayVal;
    extern decltype(popStlArray)::type popStlArrayVal;
    extern decltype(popVector)::type popVectorVal;
    extern decltype(popDeque)::type popDequeVal;
    extern decltype(popForwardList)::type popForwardListVal;
    extern decltype(popList)::type popListVal;
    extern decltype(popStack)::type popStackVal;
    extern decltype(popQueue)::type popQueueVal;
    extern decltype(popPriorityQueue)::type popPriorityQueueVal;
    extern decltype(popSet)::type popSetVal;
    extern decltype(popMultiSet)::type popMultiSetVal;
    extern decltype(popUnorderedSet)::type popUnorderedSetVal;
    extern decltype(popUnorderedMultiSet)::type popUnorderedMultiSetVal;

    extern decltype(poaStlArray)::type poaStlArrayVal;
    extern decltype(poaVector)::type poaVectorVal;
    extern decltype(poaDeque)::type poaDequeVal;
    extern decltype(poaForwardList)::type poaForwardListVal;
    extern decltype(poaList)::type poaListVal;
    extern decltype(poaSet)::type poaSetVal;
    extern decltype(poaMultiSet)::type poaMultiSetVal;
    extern decltype(poaMap)::type poaMapVal;
    extern decltype(poaMultiMap)::type poaMultiMapVal;
    extern decltype(poaUnorderedMap)::type poaUnorderedMapVal;
    extern decltype(poaUnorderedMultimap)::type poaUnorderedMultimapVal;

    extern decltype(popaStlArray)::type popaStlArrayVal;
    extern decltype(popaVector)::type popaVectorVal;
    extern decltype(popaDeque)::type popaDequeVal;
    extern decltype(popaForwardList)::type popaForwardListVal;
    extern decltype(popaList)::type popaListVal;
    extern decltype(popaSet)::type popaSetVal;
    extern decltype(popaMultiSet)::type popaMultiSetVal;
    extern decltype(popaMap)::type popaMapVal;
    extern decltype(popaMultiMap)::type popaMultiMapVal;
    extern decltype(popaUnorderedMap)::type popaUnorderedMapVal;
    extern decltype(popaUnorderedMultimap)::type popaUnorderedMultimapVal;

    extern void* validPointer;

    constexpr bool T = true; // true
    constexpr bool F = false; // false
    extern bool v; // visited
    extern std::function<void(int)> f;

}

#endif