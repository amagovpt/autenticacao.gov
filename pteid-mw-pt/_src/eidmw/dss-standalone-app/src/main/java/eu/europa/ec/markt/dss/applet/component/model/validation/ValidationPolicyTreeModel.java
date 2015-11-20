/*
 * DSS - Digital Signature Services
 *
 * Copyright (C) 2011 European Commission, Directorate-General Internal Market and Services (DG MARKT), B-1049 Bruxelles/Brussel
 *
 * Developed by: 2011 ARHS Developments S.A. (rue Nicolas Bové 2B, L-1253 Luxembourg) http://www.arhs-developments.com
 *
 * This file is part of the "DSS - Digital Signature Services" project.
 *
 * "DSS - Digital Signature Services" is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * DSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * "DSS - Digital Signature Services".  If not, see <http://www.gnu.org/licenses/>.
 */
package eu.europa.ec.markt.dss.applet.component.model.validation;

import eu.europa.ec.markt.dss.applet.component.model.AbstractTreeModel;
import eu.europa.ec.markt.dss.validation102853.engine.rules.wrapper.constraint.ValidationPolicy;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreePath;
import java.util.List;
import java.util.logging.Logger;

/**
 * Build the tree model of the validation policy
 * <p/>
 * <p/>
 * DISCLAIMER: Project owner DG-MARKT.
 *
 * @author <a href="mailto:dgmarkt.Project-DSS@arhs-developments.com">ARHS Developments</a>
 * @version $Revision: 1016 $ - $Date: 2011-06-17 15:30:45 +0200 (Fri, 17 Jun 2011) $
 */
public class ValidationPolicyTreeModel extends AbstractTreeModel<ValidationPolicyTreeRoot> {

    private static final Logger LOG = Logger.getLogger(ValidationPolicyTreeModel.class.getName());

    /**
     * The default constructor for ValidationPolicyTreeModel.
     *
     * @param validationPolicy
     */
    public ValidationPolicyTreeModel(final ValidationPolicyTreeRoot validationPolicy) {
        super(validationPolicy);

    }

    /*
     * (non-Javadoc)
     * 
     * @see eu.europa.ec.markt.dss.applet.AbstractTreeModel#getChildren(java.lang.Object)
     */
    @Override
    public List<?> getChildren(final Object object) {
        if (object instanceof ValidationPolicy) {
            ValidationPolicy validationPolicy = (ValidationPolicy) object;
            final ValidationPolicyTreeRoot validationPolicyTreeRoot = new ValidationPolicyTreeRoot(validationPolicy);
            return validationPolicyTreeRoot.getChildren();
        }
        if (object instanceof TreeNode) {
            TreeNode treeNode = (TreeNode) object;
            return treeNode.getChildren();
        }
        throw new RuntimeException("Not supported " + object);
    }

    public void fireTreeNodesRemoved(TreePath parentPath, int childIndex, Object child) {
        TreeModelEvent event = new TreeModelEvent(this, parentPath, new int[]{childIndex}, new Object[]{child});
        synchronized (listeners) {
            for (TreeModelListener listener : listeners) {
                listener.treeNodesRemoved(event);
            }
        }
    }

    public void fireTreeInsert(TreePath path, int childIndex, Object child) {
        Object[] children = {child};
//        int childIndex = this.getIndexOfChild(path.getLastPathComponent(), child);
        int[] indicies = {childIndex};
        TreeModelEvent event = new TreeModelEvent(this, path, indicies, children);
        synchronized (listeners) {
            for (TreeModelListener listener : listeners) {
                listener.treeNodesInserted(event);
            }
        }
    }
    public void fireTreeChanged(TreePath path) {
        TreeModelEvent event = new TreeModelEvent(this, path);
        synchronized (listeners) {
            for (TreeModelListener listener : listeners) {
                listener.treeNodesChanged(event);
            }
        }
    }

    public void fireTreeStructureChanged(TreePath path) {
        TreeModelEvent event = new TreeModelEvent(this, path);
        synchronized (listeners) {
            for (TreeModelListener listener : listeners) {
                listener.treeStructureChanged(event);
            }
        }
    }

}
