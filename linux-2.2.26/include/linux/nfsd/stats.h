/*
 * linux/include/nfsd/stats.h
 *
 * Statistics for NFS server.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#ifndef LINUX_NFSD_STATS_H
#define LINUX_NFSD_STATS_H

struct nfsd_stats {
	unsigned int	rchits;		/* repcache hits */
	unsigned int	rcmisses;	/* repcache hits */
	unsigned int	rcnocache;	/* uncached reqs */
	unsigned int	fh_lookup;	/* new lookup required */
	unsigned int	fh_anon;	
	unsigned int	fh_nocache_nondir;	
	unsigned int	fh_nocache_dir;	
	unsigned int	fh_stale;	/* FH stale error */
};

#ifdef __KERNEL__

extern struct nfsd_stats	nfsdstats;
extern struct svc_stat		nfsd_svcstats;

void	nfsd_stat_init(void);
void	nfsd_stat_shutdown(void);

#endif /* __KERNEL__ */
#endif /* LINUX_NFSD_STATS_H */
