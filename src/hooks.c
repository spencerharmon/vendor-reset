/*
Vendor Reset - Vendor Specific Reset
Copyright (C) 2020 Geoffrey McRae <geoff@hostfission.com>
Copyright (C) 2020 Adam Madsen <adam@ajmadsen.com>

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <linux/kernel.h>
#include <linux/pci.h>
#include "vendor-reset-dev.h"
#include "ftrace.h"
#include "hooks.h"

static int (*orig_pci_dev_specific_reset)(struct pci_dev *dev, int probe);

static int hooked_pci_dev_specific_reset(struct pci_dev *dev, int probe)
{
  int ret;
  struct vendor_reset_cfg *cfg;

  ret = orig_pci_dev_specific_reset(dev, probe);
  if (!ret || ret != -ENOTTY)
    return ret;

  cfg = vendor_reset_cfg_find(dev->vendor, dev->device);
  if (!cfg)
    return -ENOTTY;

  return vendor_reset_dev_locked(cfg, dev);
}

struct ftrace_hook fh_hooks[] = {
    HOOK("pci_dev_specific_reset", &orig_pci_dev_specific_reset, hooked_pci_dev_specific_reset),
    {0},
};
