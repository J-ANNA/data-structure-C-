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
					sorted = false;
				}
			}
			n--;
		}
}
