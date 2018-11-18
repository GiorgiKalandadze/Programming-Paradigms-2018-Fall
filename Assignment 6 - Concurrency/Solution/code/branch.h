#ifndef _BRANCH_H
#define _BRANCH_H

#include <stdint.h>
#include <semaphore.h>
#include <pthread.h>


typedef uint64_t BranchID;

typedef struct Branch {
  BranchID branchID;
  AccountAmount balance;
  int numberAccounts;
  Account   *accounts;
  pthread_mutex_t lock;
  pthread_mutex_t branch_lock;
} Branch;


int Branch_Balance(struct Bank *bank, BranchID branchID, AccountAmount *balance);
int Branch_UpdateBalance(struct Bank *bank, BranchID branchID,
                         AccountAmount change);


int Branch_Init(struct Bank *bank, int numBranches, int numAccounts,
                AccountAmount initialAmount);

int Branch_Validate(struct Bank *bank, BranchID branchID);
int Branch_Compare(Branch *branch1, Branch *branch2);


#endif /* _BRANCH_H */
