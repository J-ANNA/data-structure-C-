template <typename T>                                  //元素类型
void Vector<T>::copyFrom(T const*A, Rank lo, Rank hi)  //以数组区间A[lo,hi)为蓝本复制向量
{
    _elem = new T[_capacity = 2*(hi - lo)]; size = 0;  //分配空间，规模清零   2倍空间，预留量防止溢出
    while(lo < hi)
            _elem[size++] = A[lo++];

}

template <typename T> Vector<T>& Vector<T>::operator=(Vector<T>const& V)    //重载
{
    if(_elem)delete[]_elem;        //释放原有内容
    copyFrom(V._elem,0,V.size());   //整体复制
    return *this;                  //返回当前对象的引用，以便链式赋值
}

//扩容
template <typename T> void Vector<T>::expend()        //向量空间不足时扩容
{
	if(_size < _capacity)
		return;                                       //尚未满员时，不必扩容
	if(_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY;          //不低于最小容量
	T* oldElem = _elem;
	_elem = new T[_capacity <<= 1];                                         //容量加倍
	for(int i = 0; i< _size; i++)
	{
		_elem[i] = oldElem[i];                                              //复制原向量内容
		delete [] oldElem;                                                  //释放原空间
	}
} 

//缩容
template <typename T> void Vector<T>::shrink()        //装填因子过小时压缩向量所占空间
{
	if(_capacity < DEFAULT_CAPACITY << 1)             //不致收缩到DEFAULT_CAPACITY以下
		return;
	if(_size << 2 > _capacity)                        //以25%为界
		return;
	T* oldElem = _elem;
	_elem = new T[_capacity >>= 1];                   //容量减半
	for(int i = 0; i <_size; i++)                     //复制原向量内容
		_elem[i] = oldElem[i];
	delete [] oldElem;                               //释放原空间
}


//直接引用元素
//经重载后操作符"[]"返回的是对数组元素的引用，这就意味着可以取代get()、set()操作
template <typename T> T& Vector<T>::operator[] ( Rank r ) const //重载下标操作符 
{ return _elem[r]; } // assert: 0 <= r < _size


//置乱算法
template <typename T> void permute ( Vector<T>& V )
{ //随机置乱向量，使各元素等概率出现于各位置  
	for ( int i = V.size(); i > 0; i-- ) //自后向前
	swap ( V[i - 1], V[rand() % i] ); //V[i - 1]与V[0, i)中某一随机元素交换 
}


//区间置乱接口
template <typename T> void Vector<T>::unsort ( Rank lo, Rank hi ) { //等概率随机置乱区间[lo, hi) 
	T* V = _elem + lo; //将子向量_elem[lo, hi)视作另一向量V[0, hi - lo)
	for ( Rank i = hi - lo; i > 0; i-- ) //自后向前
	swap ( V[i - 1], V[rand() % i] ); //将V[i - 1]与V[0, i)中某一元素随机交换
}


 //判等器与比较器
 template <typename T> static bool lt ( T* a, T* b ) { return lt ( *a, *b ); } //less than
 template <typename T> static bool lt ( T& a, T& b ) { return a < b; } //less than
 template <typename T> static bool eq ( T* a, T* b ) { return eq ( *a, *b ); } //equal
 template <typename T> static bool eq ( T& a, T& b ) { return a == b; } //equal


//无序查找
template <typename T> //无序向量的顺序查找：返回最后一个元素e的位置；失败时，返回lo - 1  （lo一般为0；所以一般返回-1）
Rank Vector<T>::find ( T const& e, Rank lo, Rank hi ) const { //assert: 0 <= lo < hi <= _size
	while ( ( lo < hi-- ) && ( e != _elem[hi] ) ); //从后向前，顺序查找  若同时有多个命中元素，返回其中秩最大者
	return hi; //若hi < lo，则意味着失败；否则hi即命中元素的秩 
}

//插入
template <typename T> //将e作为秩为r元素插入
Rank Vector<T>::insert ( Rank r, T const& e ) { //assert: 0 <= r <= size
	expand(); //若有必要，扩容 
	for ( int i = _size; i > r; i-- ) _elem[i] = _elem[i-1]; //自后向前，后继元素顺次后移一个单元   插入前最后一位是 _size-1
	_elem[r] = e; _size++; //置入新元素并更新容量
	return r; //返回秩
}


//区间删除
template <typename T> int Vector<T>::remove ( Rank lo, Rank hi ) { //删除区间[lo, hi)   包含_elem[lo]，不包含_elem[hi]
if ( lo == hi ) return 0; //出于效率考虑，单独处理退化情况，比如remove(0, 0)
	while ( hi < _size ) _elem[lo++] = _elem[hi++]; //[hi, _size)顺次前移hi - lo个单元 
	_size = lo; //更新规模，直接丢弃尾部[lo, _size = hi)区间 
	shrink(); //若有必要，则缩容 
	return hi - lo; //返回被删除元素的数目 
}


//单元素删除
template <typename T> T Vector<T>::remove ( Rank r ) { //删除向量中秩为r的元素，0 <= r < size
	T e = _elem[r]; //备份被删除元素 
	remove ( r, r + 1 ); //调用区间删除算法，等效于对区间[r, r + 1)的删除
	return e; //返回被删除元素 
}


//唯一性
template <typename T> int Vector<T>::deduplicate() { //删除无序向量中重复元素（高效版）
	int oldSize = _size; //记录原规模 0003 Rank i = 1; //从_elem[1]开始 
	while ( i < _size ) //自前向后逐一考查各元素_elem[i]
	( find ( _elem[i], 0, i ) < 0 ) ? //在其前缀中寻找与之雷同者（至多一个）
	i++ : remove ( i ); //若无雷同则继续考查其后继，否则删除雷同者 
	return oldSize - _size; //向量规模变化量，即被删除元素总数 
}

//遍历    遍历操作本身只包含一层线性的循环迭代
// 方法一 
template <typename T> void Vector<T>::traverse ( void ( *visit ) ( T& ) ) //借助函数指针机制 
{ for ( int i = 0; i < _size; i++ ) visit ( _elem[i] ); } //遍历向量 

//方法二
template <typename T> template <typename VST> //元素类型、操作器 
void Vector<T>::traverse ( VST& visit ) //借助函数对象机制 
{ for ( int i = 0; i < _size; i++ ) visit ( _elem[i] ); } //遍历向量



//有序性甄别
template <typename T> int Vector<T>::disordered() const { //返回向量中逆序相邻元素对的总数 
	int n = 0; //计数器 
	for ( int i = 1; i < _size; i++ ) //逐一检查_size - 1对相邻元素 
	if ( _elem[i - 1] > _elem[i] ) n++; //逆序则计数 0005 return n; //向量有序当且仅当n = 0 
}

//有序向量唯一化
//低效版
template <typename T> int Vector<T>::uniquify() { //有序向量重复元素剔除算法（低效版） 
	int oldSize = _size; int i = 1; //当前比对元素的秩，起始于首元素 
	while ( i < _size ) //从前向后，逐一比对各对相邻元素 
	_elem[i - 1] == _elem[i] ? remove ( i ) : i++; //若雷同，则删除后者；否则，转至后一元素 
	return oldSize - _size; //向量规模变化量，即被删除元素总数 
}

//高效版   前提：向量已经排序
template <typename T> int Vector<T>::uniquify() { //有序向量重复元素剔除算法（高效版） 
	Rank i = 0, j = 0; //各对互异“相邻”元素的秩 
	while ( ++j < _size ) //逐一扫描，直至末元素   i从0开始， j从1开始
	{
		if ( _elem[i] != _elem[j] ) //跳过雷同者 
		_elem[++i] = _elem[j]; //发现不同元素时，向前移至紧邻于前者右侧 
	}	
	_size = ++i; shrink(); //直接截除尾部多余元素  若有必要进行缩容处理  _size比[i]大1
	return j - i; //向量规模变化量，即被删除元素总数 
}


//有序向量查找
template <typename T> //在有序向量的区间[lo, hi)内，确定不大于e的最后一个节点的秩 
Rank Vector<T>::search ( T const& e, Rank lo, Rank hi ) const { //assert: 0 <= lo < hi <= _size 
	return ( rand() % 2 ) ? //按各50%的概率(0(false)或1(true))随机使用二分查找或Fibonacci查找 
	binSearch ( _elem, e, lo, hi ) : fibSearch ( _elem, e, lo, hi ); 
}

//二分查找  借助递归机制
// 二分查找算法（版本A）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size 
template <typename T> static Rank binSearch ( T* A, T const& e, Rank lo, Rank hi ) { 
	while ( lo < hi ) { //每步迭代可能要做两次比较判断，有三个分支 
	Rank mi = ( lo + hi ) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移） 
	if ( e < A[mi] ) hi = mi; //深入前半段[lo, mi)继续查找 
	else if ( A[mi] < e ) lo = mi + 1; //深入后半段(mi, hi)继续查找 
	else return mi; //在mi处命中 
	} //成功查找可以提前终止 
	return -1; //查找失败 
} //有多个命中元素时，不能保证返回秩最大者；查找失败时，简单地返回-1，而不能指示失败的位置
//不足： 最短和最长分支所对应的查找长度相差约两倍


#include "fibonacci/Fib.h" //引入Fib数列类 
// Fibonacci查找算法（版本A）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size 
template <typename T> static Rank fibSearch ( T* A, T const& e, Rank lo, Rank hi )
 { 
	Fib fib ( hi - lo ); //用O(log_phi(n = hi - lo)时间创建Fib数列 
	while ( lo < hi )
	{ //每步迭代可能要做两次比较判断，有三个分支 
		while ( hi - lo < fib.get() ) fib.prev(); //通过向前顺序查找（分摊O(1)）——至多迭代几次？ 
			Rank mi = lo + fib.get() - 1; //确定形如Fib(k) - 1的轴点 
		if ( e < A[mi] ) hi = mi; //深入前半段[lo, mi)继续查找 
		else if ( A[mi] < e ) lo = mi + 1; //深入后半段(mi, hi)继续查找 
		else return mi; //在mi处命中 
	} //成功查找可以提前终止 
	return -1; //查找失败 
} //有多个命中元素时，不能保证返回秩最大者；失败时，简单地返回-1，而不能指示失败的位置

//主要区别： 在于以黄金分割点取代中点作为切分点

// 二分查找算法（版本B）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo < hi <= _size 
template <typename T> static Rank binSearch ( T* A, T const& e, Rank lo, Rank hi ) 
{ 
	while ( 1 < hi - lo ) 
	{ //每步迭代仅需做一次比较判断，有两个分支；成功查找不能提前终止 
		Rank mi = ( lo + hi ) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移） 
		( e < A[mi] ) ? hi = mi : lo = mi; //经比较后确定深入[lo, mi)或[mi, hi) 
	} //出口时hi = lo + 1，查找区间仅含一个元素A[lo]    关键！！！！
	return ( e == A[lo] ) ? lo : -1 ; //查找成功时返回对应的秩；否则统一返回-1 
} //有多个命中元素时，不能保证返回秩最大者；查找失败时，简单地返回-1，而不能指示失败的位置
//比较版本A：1、不能命中就立即返回；最好的情况效率降低，最坏的情况效率提高


// 二分查找算法（版本C）：在有序向量的区间[lo, hi)内查找元素e，0 <= lo <= hi <= _size 
template <typename T> static Rank binSearch ( T* A, T const& e, Rank lo, Rank hi )
{ 
	while ( lo < hi ) 
	{ //每步迭代仅需做一次比较判断，有两个分支 
		Rank mi = ( lo + hi ) >> 1; //以中点为轴点（区间宽度的折半，等效于宽度之数值表示的右移） 
		( e < A[mi] ) ? hi = mi : lo = mi + 1; //经比较后确定深入[lo, mi)或(mi, hi) 
	} //成功查找不能提前终止 
	return --lo; //循环结束时，lo为大于e的元素的最小秩，故lo - 1即不大于e的元素的最大秩  ！！！！
} //有多个命中元素时，总能保证返回秩最大者；查找失败时，能够返回失败的位置




//排序
template <typename T> void Vector<T>::sort ( Rank lo, Rank hi ) 
{ //向量区间[lo, hi)排序 
	switch ( rand() % 5 )
	{ //随机选取排序算法。可根据具体问题的特点灵活选取或扩充 
	case 1: bubbleSort ( lo, hi ); break; //起泡排序 
	case 2: selectionSort ( lo, hi ); break; //选择排序（习题） 
	case 3: mergeSort ( lo, hi ); break; //归并排序 
	case 4: heapSort ( lo, hi ); break; //堆排序（稍后介绍） 
	default: quickSort ( lo, hi ); break; //快速排序（稍后介绍） 
	} 
}

//起泡排序
0001 template <typename T> //向量的起泡排序 
void Vector<T>::bubbleSort ( Rank lo, Rank hi ) //assert: 0 <= lo < hi <= size 
{
	while ( !bubble ( lo, hi-- ) );
} //逐趟做扫描交换，直至全序

template <typename T> bool Vector<T>::bubble ( Rank lo, Rank hi ) 
{ //一趟扫描交换 
	bool sorted = true; //整体有序标志 
	while ( ++lo < hi ) //自左向右，逐一检查各对相邻元素 
		if ( _elem[lo - 1] > _elem[lo] ) 
		{ //若逆序，则 
			sorted = false; //意味着尚未整体有序，并需要 
			swap ( _elem[lo - 1], _elem[lo] ); //通过交换使局部有序 
		} 
	return sorted; //返回有序标志 
}



//归并排序
//分治策略
template <typename T>                              //向量归并排序
void Vector<T>::mergeSort(Rank lo, Rank hi)        //0 <= lo < hi <= size
{
	if(hi - lo < 2) return;                        //单元素区间自然有序，否则...
	int mi = (lo + hi)/2;                          //以中点为界
	mergeSort(lo,mi); mergeSort(mi,hi);            //分别排序
	merge(lo,mi,hi);                               //归并
}
//二路归并接口的实现
template <typename T>                              //有序向量的归并
void Vector<T>::merge(Rank lo, Rank mi, Rank hi)   //各自有序的子向量[lo,mi)和[mi,hi)
{
	T* A = _elem + lo;                             //合并后的向量A[0,hi - lo] = _elem[lo,hi)
	int lb = mi - lo; T* B = new T[lb]；            //前子向量B[0,lb) = _elem[lo,mi)
	for(Rank i = 0; i < lb; B[i] = A[i++]);         //复制前子向量 （该写法在Qt上测试有误，B[0]为空，B[1] = A[0]）
	int lc = hi - mi; T* C = _elem + mi;             //后子向量C[0,lc) = _elem[mi,hi)
	for(Rank i = 0, j = 0,k = 0; (j < lb) || (k < lc);)                        //B[j]和C[k]中的小者续至A末尾
	{
		if((j < lb) && (!(k < lc) || (B[j] <= C[k]))) A[i++] = B[j++];
		if((k < lc) && (!(j < lb) || (C[k] <  B[j]))) A[i++] = C[k++]; 
	}	
	delete [] B;  //释放临时空间B
	
}        //归并后得到完整的有序向量[lo,hi)





































