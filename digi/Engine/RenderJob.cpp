#include "RenderJob.h"


namespace digi {

void renderSorted(RenderJob* renderJobs)
{
	//ResetShader resetShader = &resetShaderDummy;

	// list merge sort by Simon Tatham
	typedef RenderJob Element;
	Element* list = renderJobs;
	
	int insize = 1;

	while (true)
	{
		Element* p = list;
		list = NULL;
		Element* tail = NULL;

		int nmerges = 0;  /* count number of merges we do in this pass */

		while (p != NULL)
		{
			nmerges++;  /* there exists a merge to be done */
			
			/* step `insize' places along from p */
			Element* q = p;
			int psize = 0;
			for (int i = 0; i < insize; i++)
			{
				psize++;
				q = q->next;
				if (q == NULL)
					break;
			}

			/* if q hasn't fallen off end, we have two lists to merge */
			int qsize = insize;

			/* now we have two lists; merge them */
			Element* e;
			while (psize > 0 || (qsize > 0 && q != NULL))
			{
				/* decide whether next element of merge comes from p or q */
				if (psize == 0)
				{
					/* p is empty; e must come from q. */
					e = q; q = q->next; qsize--;
				}
				else if (qsize == 0 || q == NULL)
				{
					/* q is empty; e must come from p. */
					e = p; p = p->next; psize--;
				}
				else if (p->distance > q->distance)
				{
					/* First element of p is lower (or same); e must come from p. */
					e = p; p = p->next; psize--;
				}
				else
				{
					/* First element of q is lower; e must come from q. */
					e = q; q = q->next; qsize--;
				}

				/* add the next element to the merged list */
				if (tail != NULL)
					tail->next = e;
				else
					list = e;

				tail = e;
			}

			/* now p has stepped `insize' places along, and q has too */
			p = q;
		}
		tail->next = NULL;

		/* If we have done only one merge, we're finished. */
		if (nmerges <= 1)   /* allow for nmerges==0, the empty list case */
			break;

		/* Otherwise repeat, merging lists twice the size */
		insize *= 2;
	}


	while (list != NULL)
	{
		//resetShader = list->render(list, resetShader);
		list->render(list);
		list = list->next;
	}
}
/*
void resetShaderDummy()
{
}
*/
} // namespace digi
