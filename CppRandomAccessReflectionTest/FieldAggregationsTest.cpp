#include "FieldAggregationsTest.h"

namespace FieldAggregations
{
    PrimitiveField primitive;
    PrimitivePointerField primitivePointer;

    ObjectField object;
    ObjectPointerField objectPointer;

    IP_Array ipArray;
    IP_StlArray ipStlArray;
    IP_Vector ipVector;
    IP_Deque ipDeque;
    IP_ForwardList ipForwardList;
    IP_List ipList;
    IP_Stack ipStack;
    IP_Queue ipQueue;
    IP_PriorityQueue ipPriorityQueue;
    IP_Set ipSet;
    IP_MultiSet ipMultiSet;
    IP_UnorderedSet ipUnorderedSet;
    IP_UnorderedMultiSet ipUnorderedMultiSet;

    IPP_Array ippArray;
    IPP_StlArray ippStlArray;
    IPP_Vector ippVector;
    IPP_Deque ippDeque;
    IPP_ForwardList ippForwardList;
    IPP_List ippList;
    IPP_Stack ippStack;
    IPP_Queue ippQueue;
    IPP_PriorityQueue ippPriorityQueue;
    IPP_Set ippSet;
    IPP_MultiSet ippMultiSet;
    IPP_UnorderedSet ippUnorderedSet;
    IPP_UnorderedMultiSet ippUnorderedMultiSet;

    IPA_Array ipaArray;
    IPA_StlArray ipaStlArray;
    IPA_Vector ipaVector;
    IPA_Deque ipaDeque;
    IPA_ForwardList ipaForwardList;
    IPA_List ipaList;
    IPA_Set ipaSet;
    IPA_MultiSet ipaMultiSet;
    IPA_Map ipaMap;
    IPA_MultiMap ipaMultiMap;
    IPA_UnorderedMap ipaUnorderedMap;
    IPA_UnorderedMultimap ipaUnorderedMultimap;

    IPPA_Array ippaArray;
    IPPA_StlArray ippaStlArray;
    IPPA_Vector ippaVector;
    IPPA_Deque ippaDeque;
    IPPA_ForwardList ippaForwardList;
    IPPA_List ippaList;
    IPPA_Set ippaSet;
    IPPA_MultiSet ippaMultiSet;
    IPPA_Map ippaMap;
    IPPA_MultiMap ippaMultiMap;
    IPPA_UnorderedMap ippaUnorderedMap;
    IPPA_UnorderedMultimap ippaUnorderedMultimap;

    IO_Array ioArray;
    IO_StlArray ioStlArray;
    IO_Vector ioVector;
    IO_Deque ioDeque;
    IO_ForwardList ioForwardList;
    IO_List ioList;
    IO_Stack ioStack;
    IO_Queue ioQueue;
    IO_PriorityQueue ioPriorityQueue;
    IO_Set ioSet;
    IO_MultiSet ioMultiSet;
    IO_UnorderedSet ioUnorderedSet;
    IO_UnorderedMultiSet ioUnorderedMultiSet;

    IOP_Array iopArray;
    IOP_StlArray iopStlArray;
    IOP_Vector iopVector;
    IOP_Deque iopDeque;
    IOP_ForwardList iopForwardList;
    IOP_List iopList;
    IOP_Stack iopStack;
    IOP_Queue iopQueue;
    IOP_PriorityQueue iopPriorityQueue;
    IOP_Set iopSet;
    IOP_MultiSet iopMultiSet;
    IOP_UnorderedSet iopUnorderedSet;
    IOP_UnorderedMultiSet iopUnorderedMultiSet;

    IOA_Array ioaArray;
    IOA_StlArray ioaStlArray;
    IOA_Vector ioaVector;
    IOA_Deque ioaDeque;
    IOA_ForwardList ioaForwardList;
    IOA_List ioaList;
    IOA_Set ioaSet;
    IOA_MultiSet ioaMultiSet;
    IOA_Map ioaMap;
    IOA_MultiMap ioaMultiMap;
    IOA_UnorderedMap ioaUnorderedMap;
    IOA_UnorderedMultimap ioaUnorderedMultimap;

    IOPA_Array iopaArray;
    IOPA_StlArray iopaStlArray;
    IOPA_Vector iopaVector;
    IOPA_Deque iopaDeque;
    IOPA_ForwardList iopaForwardList;
    IOPA_List iopaList;
    IOPA_Set iopaSet;
    IOPA_MultiSet iopaMultiSet;
    IOPA_Map iopaMap;
    IOPA_MultiMap iopaMultiMap;
    IOPA_UnorderedMap iopaUnorderedMap;
    IOPA_UnorderedMultimap iopaUnorderedMultimap;



    PP_Array ppArray;
    PP_StlArray ppStlArray;
    PP_Vector ppVector;
    PP_Deque ppDeque;
    PP_ForwardList ppForwardList;
    PP_List ppList;
    PP_Stack ppStack;
    PP_Queue ppQueue;
    PP_PriorityQueue ppPriorityQueue;
    PP_Set ppSet;
    PP_MultiSet ppMultiSet;
    PP_UnorderedSet ppUnorderedSet;
    PP_UnorderedMultiSet ppUnorderedMultiSet;

    PPP_Array pppArray;
    PPP_StlArray pppStlArray;
    PPP_Vector pppVector;
    PPP_Deque pppDeque;
    PPP_ForwardList pppForwardList;
    PPP_List pppList;
    PPP_Stack pppStack;
    PPP_Queue pppQueue;
    PPP_PriorityQueue pppPriorityQueue;
    PPP_Set pppSet;
    PPP_MultiSet pppMultiSet;
    PPP_UnorderedSet pppUnorderedSet;
    PPP_UnorderedMultiSet pppUnorderedMultiSet;

    PPA_Array ppaArray;
    PPA_StlArray ppaStlArray;
    PPA_Vector ppaVector;
    PPA_Deque ppaDeque;
    PPA_ForwardList ppaForwardList;
    PPA_List ppaList;
    PPA_Set ppaSet;
    PPA_MultiSet ppaMultiSet;
    PPA_Map ppaMap;
    PPA_MultiMap ppaMultiMap;
    PPA_UnorderedMap ppaUnorderedMap;
    PPA_UnorderedMultimap ppaUnorderedMultimap;

    PPPA_Array pppaArray;
    PPPA_StlArray pppaStlArray;
    PPPA_Vector pppaVector;
    PPPA_Deque pppaDeque;
    PPPA_ForwardList pppaForwardList;
    PPPA_List pppaList;
    PPPA_Set pppaSet;
    PPPA_MultiSet pppaMultiSet;
    PPPA_Map pppaMap;
    PPPA_MultiMap pppaMultiMap;
    PPPA_UnorderedMap pppaUnorderedMap;
    PPPA_UnorderedMultimap pppaUnorderedMultimap;

    PO_Array poArray;
    PO_StlArray poStlArray;
    PO_Vector poVector;
    PO_Deque poDeque;
    PO_ForwardList poForwardList;
    PO_List poList;
    PO_Stack poStack;
    PO_Queue poQueue;
    PO_PriorityQueue poPriorityQueue;
    PO_Set poSet;
    PO_MultiSet poMultiSet;
    PO_UnorderedSet poUnorderedSet;
    PO_UnorderedMultiSet poUnorderedMultiSet;

    POP_Array popArray;
    POP_StlArray popStlArray;
    POP_Vector popVector;
    POP_Deque popDeque;
    POP_ForwardList popForwardList;
    POP_List popList;
    POP_Stack popStack;
    POP_Queue popQueue;
    POP_PriorityQueue popPriorityQueue;
    POP_Set popSet;
    POP_MultiSet popMultiSet;
    POP_UnorderedSet popUnorderedSet;
    POP_UnorderedMultiSet popUnorderedMultiSet;

    POA_Array poaArray;
    POA_StlArray poaStlArray;
    POA_Vector poaVector;
    POA_Deque poaDeque;
    POA_ForwardList poaForwardList;
    POA_List poaList;
    POA_Set poaSet;
    POA_MultiSet poaMultiSet;
    POA_Map poaMap;
    POA_MultiMap poaMultiMap;
    POA_UnorderedMap poaUnorderedMap;
    POA_UnorderedMultimap poaUnorderedMultimap;

    POPA_Array popaArray;
    POPA_StlArray popaStlArray;
    POPA_Vector popaVector;
    POPA_Deque popaDeque;
    POPA_ForwardList popaForwardList;
    POPA_List popaList;
    POPA_Set popaSet;
    POPA_MultiSet popaMultiSet;
    POPA_Map popaMap;
    POPA_MultiMap popaMultiMap;
    POPA_UnorderedMap popaUnorderedMap;
    POPA_UnorderedMultimap popaUnorderedMultimap;



    decltype(primitive)::Type primitiveVal;

    decltype(primitivePointer)::Type primitivePointerVal = nullptr;

    decltype(object)::Type objectVal;

    decltype(objectPointer)::Type objectPointerVal = nullptr;


    decltype(ipArray)::Type ipArrayVal;
    decltype(ipStlArray)::Type ipStlArrayVal;
    decltype(ipVector)::Type ipVectorVal;
    decltype(ipDeque)::Type ipDequeVal;
    decltype(ipForwardList)::Type ipForwardListVal;
    decltype(ipList)::Type ipListVal;
    decltype(ipStack)::Type ipStackVal;
    decltype(ipQueue)::Type ipQueueVal;
    decltype(ipPriorityQueue)::Type ipPriorityQueueVal;
    decltype(ipSet)::Type ipSetVal;
    decltype(ipMultiSet)::Type ipMultiSetVal;
    decltype(ipUnorderedSet)::Type ipUnorderedSetVal;
    decltype(ipUnorderedMultiSet)::Type ipUnorderedMultiSetVal;

    decltype(ippArray)::Type ippArrayVal;
    decltype(ippStlArray)::Type ippStlArrayVal;
    decltype(ippVector)::Type ippVectorVal;
    decltype(ippDeque)::Type ippDequeVal;
    decltype(ippForwardList)::Type ippForwardListVal;
    decltype(ippList)::Type ippListVal;
    decltype(ippStack)::Type ippStackVal;
    decltype(ippQueue)::Type ippQueueVal;
    decltype(ippPriorityQueue)::Type ippPriorityQueueVal;
    decltype(ippSet)::Type ippSetVal;
    decltype(ippMultiSet)::Type ippMultiSetVal;
    decltype(ippUnorderedSet)::Type ippUnorderedSetVal;
    decltype(ippUnorderedMultiSet)::Type ippUnorderedMultiSetVal;

    decltype(ipaStlArray)::Type ipaStlArrayVal;
    decltype(ipaVector)::Type ipaVectorVal;
    decltype(ipaDeque)::Type ipaDequeVal;
    decltype(ipaForwardList)::Type ipaForwardListVal;
    decltype(ipaList)::Type ipaListVal;
    decltype(ipaSet)::Type ipaSetVal;
    decltype(ipaMultiSet)::Type ipaMultiSetVal;
    decltype(ipaMap)::Type ipaMapVal;
    decltype(ipaMultiMap)::Type ipaMultiMapVal;
    decltype(ipaUnorderedMap)::Type ipaUnorderedMapVal;
    decltype(ipaUnorderedMultimap)::Type ipaUnorderedMultimapVal;

    decltype(ippaStlArray)::Type ippaStlArrayVal;
    decltype(ippaVector)::Type ippaVectorVal;
    decltype(ippaDeque)::Type ippaDequeVal;
    decltype(ippaForwardList)::Type ippaForwardListVal;
    decltype(ippaList)::Type ippaListVal;
    decltype(ippaSet)::Type ippaSetVal;
    decltype(ippaMultiSet)::Type ippaMultiSetVal;
    decltype(ippaMap)::Type ippaMapVal;
    decltype(ippaMultiMap)::Type ippaMultiMapVal;
    decltype(ippaUnorderedMap)::Type ippaUnorderedMapVal;
    decltype(ippaUnorderedMultimap)::Type ippaUnorderedMultimapVal;

    decltype(ioArray)::Type ioArrayVal;
    decltype(ioStlArray)::Type ioStlArrayVal;
    decltype(ioVector)::Type ioVectorVal;
    decltype(ioDeque)::Type ioDequeVal;
    decltype(ioForwardList)::Type ioForwardListVal;
    decltype(ioList)::Type ioListVal;
    decltype(ioStack)::Type ioStackVal;
    decltype(ioQueue)::Type ioQueueVal;
    decltype(ioPriorityQueue)::Type ioPriorityQueueVal;
    decltype(ioSet)::Type ioSetVal;
    decltype(ioMultiSet)::Type ioMultiSetVal;
    decltype(ioUnorderedSet)::Type ioUnorderedSetVal;
    decltype(ioUnorderedMultiSet)::Type ioUnorderedMultiSetVal;

    decltype(iopArray)::Type iopArrayVal;
    decltype(iopStlArray)::Type iopStlArrayVal;
    decltype(iopVector)::Type iopVectorVal;
    decltype(iopDeque)::Type iopDequeVal;
    decltype(iopForwardList)::Type iopForwardListVal;
    decltype(iopList)::Type iopListVal;
    decltype(iopStack)::Type iopStackVal;
    decltype(iopQueue)::Type iopQueueVal;
    decltype(iopPriorityQueue)::Type iopPriorityQueueVal;
    decltype(iopSet)::Type iopSetVal;
    decltype(iopMultiSet)::Type iopMultiSetVal;
    decltype(iopUnorderedSet)::Type iopUnorderedSetVal;
    decltype(iopUnorderedMultiSet)::Type iopUnorderedMultiSetVal;

    decltype(ioaStlArray)::Type ioaStlArrayVal;
    decltype(ioaVector)::Type ioaVectorVal;
    decltype(ioaDeque)::Type ioaDequeVal;
    decltype(ioaForwardList)::Type ioaForwardListVal;
    decltype(ioaList)::Type ioaListVal;
    decltype(ioaSet)::Type ioaSetVal;
    decltype(ioaMultiSet)::Type ioaMultiSetVal;
    decltype(ioaMap)::Type ioaMapVal;
    decltype(ioaMultiMap)::Type ioaMultiMapVal;
    decltype(ioaUnorderedMap)::Type ioaUnorderedMapVal;
    decltype(ioaUnorderedMultimap)::Type ioaUnorderedMultimapVal;

    decltype(iopaStlArray)::Type iopaStlArrayVal;
    decltype(iopaVector)::Type iopaVectorVal;
    decltype(iopaDeque)::Type iopaDequeVal;
    decltype(iopaForwardList)::Type iopaForwardListVal;
    decltype(iopaList)::Type iopaListVal;
    decltype(iopaSet)::Type iopaSetVal;
    decltype(iopaMultiSet)::Type iopaMultiSetVal;
    decltype(iopaMap)::Type iopaMapVal;
    decltype(iopaMultiMap)::Type iopaMultiMapVal;
    decltype(iopaUnorderedMap)::Type iopaUnorderedMapVal;
    decltype(iopaUnorderedMultimap)::Type iopaUnorderedMultimapVal;

    decltype(ppArray)::Type ppArrayVal = nullptr;
    decltype(ppStlArray)::Type ppStlArrayVal = nullptr;
    decltype(ppVector)::Type ppVectorVal = nullptr;
    decltype(ppDeque)::Type ppDequeVal = nullptr;
    decltype(ppForwardList)::Type ppForwardListVal = nullptr;
    decltype(ppList)::Type ppListVal = nullptr;
    decltype(ppStack)::Type ppStackVal = nullptr;
    decltype(ppQueue)::Type ppQueueVal = nullptr;
    decltype(ppPriorityQueue)::Type ppPriorityQueueVal = nullptr;
    decltype(ppSet)::Type ppSetVal = nullptr;
    decltype(ppMultiSet)::Type ppMultiSetVal = nullptr;
    decltype(ppUnorderedSet)::Type ppUnorderedSetVal = nullptr;
    decltype(ppUnorderedMultiSet)::Type ppUnorderedMultiSetVal = nullptr;

    decltype(pppArray)::Type pppArrayVal = nullptr;
    decltype(pppStlArray)::Type pppStlArrayVal = nullptr;
    decltype(pppVector)::Type pppVectorVal = nullptr;
    decltype(pppDeque)::Type pppDequeVal = nullptr;
    decltype(pppForwardList)::Type pppForwardListVal = nullptr;
    decltype(pppList)::Type pppListVal = nullptr;
    decltype(pppStack)::Type pppStackVal = nullptr;
    decltype(pppQueue)::Type pppQueueVal = nullptr;
    decltype(pppPriorityQueue)::Type pppPriorityQueueVal = nullptr;
    decltype(pppSet)::Type pppSetVal = nullptr;
    decltype(pppMultiSet)::Type pppMultiSetVal = nullptr;
    decltype(pppUnorderedSet)::Type pppUnorderedSetVal = nullptr;
    decltype(pppUnorderedMultiSet)::Type pppUnorderedMultiSetVal = nullptr;

    decltype(ppaStlArray)::Type ppaStlArrayVal = nullptr;
    decltype(ppaVector)::Type ppaVectorVal = nullptr;
    decltype(ppaDeque)::Type ppaDequeVal = nullptr;
    decltype(ppaForwardList)::Type ppaForwardListVal = nullptr;
    decltype(ppaList)::Type ppaListVal = nullptr;
    decltype(ppaSet)::Type ppaSetVal = nullptr;
    decltype(ppaMultiSet)::Type ppaMultiSetVal = nullptr;
    decltype(ppaMap)::Type ppaMapVal = nullptr;
    decltype(ppaMultiMap)::Type ppaMultiMapVal = nullptr;
    decltype(ppaUnorderedMap)::Type ppaUnorderedMapVal = nullptr;
    decltype(ppaUnorderedMultimap)::Type ppaUnorderedMultimapVal = nullptr;

    decltype(pppaStlArray)::Type pppaStlArrayVal = nullptr;
    decltype(pppaVector)::Type pppaVectorVal = nullptr;
    decltype(pppaDeque)::Type pppaDequeVal = nullptr;
    decltype(pppaForwardList)::Type pppaForwardListVal = nullptr;
    decltype(pppaList)::Type pppaListVal = nullptr;
    decltype(pppaSet)::Type pppaSetVal = nullptr;
    decltype(pppaMultiSet)::Type pppaMultiSetVal = nullptr;
    decltype(pppaMap)::Type pppaMapVal = nullptr;
    decltype(pppaMultiMap)::Type pppaMultiMapVal = nullptr;
    decltype(pppaUnorderedMap)::Type pppaUnorderedMapVal = nullptr;
    decltype(pppaUnorderedMultimap)::Type pppaUnorderedMultimapVal = nullptr;

    decltype(poArray)::Type poArrayVal = nullptr;
    decltype(poStlArray)::Type poStlArrayVal = nullptr;
    decltype(poVector)::Type poVectorVal = nullptr;
    decltype(poDeque)::Type poDequeVal = nullptr;
    decltype(poForwardList)::Type poForwardListVal = nullptr;
    decltype(poList)::Type poListVal = nullptr;
    decltype(poStack)::Type poStackVal = nullptr;
    decltype(poQueue)::Type poQueueVal = nullptr;
    decltype(poPriorityQueue)::Type poPriorityQueueVal = nullptr;
    decltype(poSet)::Type poSetVal = nullptr;
    decltype(poMultiSet)::Type poMultiSetVal = nullptr;
    decltype(poUnorderedSet)::Type poUnorderedSetVal = nullptr;
    decltype(poUnorderedMultiSet)::Type poUnorderedMultiSetVal = nullptr;

    decltype(popArray)::Type popArrayVal = nullptr;
    decltype(popStlArray)::Type popStlArrayVal = nullptr;
    decltype(popVector)::Type popVectorVal = nullptr;
    decltype(popDeque)::Type popDequeVal = nullptr;
    decltype(popForwardList)::Type popForwardListVal = nullptr;
    decltype(popList)::Type popListVal = nullptr;
    decltype(popStack)::Type popStackVal = nullptr;
    decltype(popQueue)::Type popQueueVal = nullptr;
    decltype(popPriorityQueue)::Type popPriorityQueueVal = nullptr;
    decltype(popSet)::Type popSetVal = nullptr;
    decltype(popMultiSet)::Type popMultiSetVal = nullptr;
    decltype(popUnorderedSet)::Type popUnorderedSetVal = nullptr;
    decltype(popUnorderedMultiSet)::Type popUnorderedMultiSetVal = nullptr;

    decltype(poaStlArray)::Type poaStlArrayVal = nullptr;
    decltype(poaVector)::Type poaVectorVal = nullptr;
    decltype(poaDeque)::Type poaDequeVal = nullptr;
    decltype(poaForwardList)::Type poaForwardListVal = nullptr;
    decltype(poaList)::Type poaListVal = nullptr;
    decltype(poaSet)::Type poaSetVal = nullptr;
    decltype(poaMultiSet)::Type poaMultiSetVal = nullptr;
    decltype(poaMap)::Type poaMapVal = nullptr;
    decltype(poaMultiMap)::Type poaMultiMapVal = nullptr;
    decltype(poaUnorderedMap)::Type poaUnorderedMapVal = nullptr;
    decltype(poaUnorderedMultimap)::Type poaUnorderedMultimapVal = nullptr;

    decltype(popaStlArray)::Type popaStlArrayVal = nullptr;
    decltype(popaVector)::Type popaVectorVal = nullptr;
    decltype(popaDeque)::Type popaDequeVal = nullptr;
    decltype(popaForwardList)::Type popaForwardListVal = nullptr;
    decltype(popaList)::Type popaListVal = nullptr;
    decltype(popaSet)::Type popaSetVal = nullptr;
    decltype(popaMultiSet)::Type popaMultiSetVal = nullptr;
    decltype(popaMap)::Type popaMapVal = nullptr;
    decltype(popaMultiMap)::Type popaMultiMapVal = nullptr;
    decltype(popaUnorderedMap)::Type popaUnorderedMapVal = nullptr;
    decltype(popaUnorderedMultimap)::Type popaUnorderedMultimapVal = nullptr;

    int someInt = 0;
    void* validPointer = &someInt;

    bool v = false; // visited
    std::function<void(int)> f([&](int){v=T;});
    
    TEST(FieldAggregationsTest, IfPointer)
    {
        v=is_pointable<decltype(primitiveVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(primitivePointerVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(objectVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(objectPointerVal)>::value; EXPECT_TRUE(v);


        v=is_pointable<decltype(ipArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipStackVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipPriorityQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipUnorderedSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipUnorderedMultiSetVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(ippArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippStackVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippPriorityQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippUnorderedSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippUnorderedMultiSetVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(ipaStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaMultiMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaUnorderedMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ipaUnorderedMultimapVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(ippaStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaMultiMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaUnorderedMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ippaUnorderedMultimapVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(ioArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioStackVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioPriorityQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioUnorderedSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioUnorderedMultiSetVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(iopArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopStackVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopPriorityQueueVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopUnorderedSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopUnorderedMultiSetVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(ioaStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaMultiMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaUnorderedMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(ioaUnorderedMultimapVal)>::value; EXPECT_FALSE(v);

        v=is_pointable<decltype(iopaStlArrayVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaVectorVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaDequeVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaForwardListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaListVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaMultiSetVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaMultiMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaUnorderedMapVal)>::value; EXPECT_FALSE(v);
        v=is_pointable<decltype(iopaUnorderedMultimapVal)>::value; EXPECT_FALSE(v);


        v=is_pointable<decltype(ppArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppStackVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppPriorityQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppUnorderedSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppUnorderedMultiSetVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(pppArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppStackVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppPriorityQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppUnorderedSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppUnorderedMultiSetVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(ppaStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaMultiMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaUnorderedMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(ppaUnorderedMultimapVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(pppaStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaMultiMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaUnorderedMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(pppaUnorderedMultimapVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(poArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poStackVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poPriorityQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poUnorderedSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poUnorderedMultiSetVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(popArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popStackVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popPriorityQueueVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popUnorderedSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popUnorderedMultiSetVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(poaStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaMultiMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaUnorderedMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(poaUnorderedMultimapVal)>::value; EXPECT_TRUE(v);

        v=is_pointable<decltype(popaStlArrayVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaVectorVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaDequeVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaForwardListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaListVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaMultiSetVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaMultiMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaUnorderedMapVal)>::value; EXPECT_TRUE(v);
        v=is_pointable<decltype(popaUnorderedMultimapVal)>::value; EXPECT_TRUE(v);
    }

};
