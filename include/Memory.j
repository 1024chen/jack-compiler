class Memory
{
    static Array freelist;      // ��ʱfreelistʵ���Ͼ��Ǹ�ָ��

    /** ��ʼ���ڴ���� */
    function void init() 
    {
        freelist = 0;
        freelist[2048] = 14334; // 14334 = 37FE(H)
        freelist[2049] = 2050;  // 2050 = 802(H)
        return;
    }
    
    /** ���ص�ַΪaddress���ڴ浥Ԫ */
    function int peek(int address) 
    {
        return freelist[address];
    }

    /** ������ֵvalue�����ַΪaddress���ڴ浥Ԫ�������������ַ�ϵ����ڴ��ֵ */
    function void poke(int address, int value) 
    {
        freelist[address] = value;
        return;
    }
    
    /** ���ڴ����Ѱ�Ҳ�����һ���СΪsize���ڴ���, ���������ַ��ָ�� */
    function int alloc(int size) 
    {
        int listRoom, listTag, tempAdd, returnVal;
        int minSize;
        
        minSize = size + 3;
        listTag = 2048;
        listRoom = Memory.peek(listTag);
        while(minSize > listRoom)
        {
            listTag = listTag + 1;
            listTag = Memory.peek(listTag);
            listRoom = Memory.peek(listTag);
//            if(listTag == 0) 
//            {
//                Sys.error(7);
//            }
        }
        returnVal = listTag + 2;
        Memory.poke(listTag, 0);
        listTag = listTag + 1;
        tempAdd = Memory.peek(listTag) + size;
        Memory.poke(listTag, tempAdd);
        Memory.poke(tempAdd, listRoom - size - 2);
        listTag = tempAdd + 1;
        Memory.poke(listTag, listTag + 1);          
        return returnVal;
    }
    
    /** �ջ�֮ǰ�����������ڴ�ռ� */
    function void deAlloc(int object) 
    {
        int length;
        length = Memory.peek(object - 1) - object;
        Memory.poke(object - 2, length);
        return;
    }
}
