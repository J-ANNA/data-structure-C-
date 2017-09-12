void bubblesort1A(int arrayA[];int n)
{
	 bool sorted = false;
	 while (!sorted) {
			sorted = true;
			for(int i = 1;i < n;i++)
			{
				if(arrayA[i-1]>arrayA[i])
				{
	//                swap(arrayA[i-1],arrayA[i]);
					int temp;
					temp = arrayA[i-1];
					arrayA[i-1] = arrayA[i];
					arrayA[i] = temp;
					sorted = false;          //因整体排序不能保证，需要清除排序标志
				}
			}
			n--;        //至此末元素必然就位，故可以缩短排序序列的有效长度
		}
}
